# Tilegame Architecture

Tilegame is a 2D, tile-based RPG engine in the style of classic top-down Zelda
and Pokemon games. It is written in C++20 and embeds Lua (via
[sol3](https://github.com/ThePhD/sol2)) so that game *content* — maps,
characters, dialogue, and story logic — can eventually be authored entirely in
Lua on top of a fixed C++ runtime.

The long-term goal is an engine that is **ECS-first end to end**: C++ owns the
main loop, rendering, and a fixed set of systems, while everything that makes
a specific game a specific game (its story, its NPC behavior, its triggers and
events) is expressed as entity/component data and Lua callbacks. A full RPG
has two pillars, story and battles; this codebase currently focuses on story,
with a battle system intentionally out of scope for now.

## Repository layout

```
engine/         Reusable engine library (static lib "engine")
  include/core       Windowing, math/shapes, resources, shaders, scene graph
  include/graphics   Sprite batching, post-processing pipeline
  include/tilemap    Tiled-map data model (loaded via the tileson library)
  src/               Implementation matching the above

tilegame/       The actual game executable ("tilegame")
  src/               ECS components, systems, the Lua bridge, main loop glue
  src/components/    Plain-data components attached to entt entities
  src/systems/       Per-frame logic operating on components
  src/worldscene/    Owns the entt::registry and all systems for the game world
  content/           Maps, tilesets, textures, shaders, and Lua scripts

vendor/         Third-party/vendored dependencies (entt, sol3, entt_sol,
                 glad, stb_image, tileson, debugger.lua, inspect.lua)
```

`engine` knows nothing about game rules, ECS components, or Lua — it only
provides windowing, rendering primitives, resource loading, and tilemap data
structures. All game-specific logic lives in `tilegame`.

## Main loop

`engine::Game` (`engine/include/core/game.hpp`) owns the window, the
`GraphicsDevice`, and the `ResourceManager`, and drives a classic
initialize → load_content → (update/draw loop) → unload_content lifecycle via
`Game::run()`.

`tilegame::Tilegame` (`tilegame/src/tilegame.hpp`) subclasses `engine::Game`
and owns a `SpriteBatch` plus a single `WorldScene`. `WorldScene`
(`tilegame/src/worldscene/worldscene.hpp`) is the actual per-frame
orchestrator: it owns the `entt::registry` and every `System` instance, and
calls them in a fixed, hand-ordered sequence each frame
(`WorldScene::update`), e.g.:

```
Daytime → Timer → Particle → Player → Script → MovementController
        → CollisionDetection → Movement → Animation → Pin → Camera
```

The ordering encodes real dependencies (e.g. `Player`/`Script` decide a
*direction* for an entity to move in; `MovementController` turns that into a
velocity; `CollisionDetection` clips that velocity against the tilemap;
`Movement` applies the final, clipped velocity to the entity's transform).

There is currently one `Scene` (`WorldScene`); `engine::Scene<T>` is a
template so the engine side is scene-graph-data-agnostic, but the game only
instantiates a single scene.

## Entity-Component-System

Tilegame uses [EnTT](https://github.com/skypjack/entt) (`vendor/entt`) as its
ECS. Two conventions run through the whole codebase:

- **Components are plain structs** under `tilegame/src/components/`, each
  typically with its own `.hpp`/`.cpp` pair. Structs stay data-only; behavior
  lives in systems.
- **Systems are classes** under `tilegame/src/systems/`, each deriving from
  the common `tilegame::systems::System` base
  (`tilegame/src/systems/system.hpp`). A system's constructor takes the
  `Scene` and `entt::registry`; its `update(GameTime)` runs once per frame and
  operates on `entt::view`/`entt::group` queries over components.

### Events

Some components are **event components**: short-lived, one-frame markers such
as `TimerEvent` or `TargetReachedEvent`
(`tilegame/src/components/timer.hpp`, `tilegame/src/components/target.hpp`).
A system creates one of these as a component on the *source* entity for the
frame in which the event fires. `System::raise_event<EventType>()`
(`tilegame/src/systems/system.hpp`) then delivers each event to every
`EventListener<EventType>` component in the registry — a listener wraps a
callback (C++ `std::function`, or in practice a Lua function, see below) and
an optional source-entity filter. Event components are cleared again at
`end_update()` so they only exist for a single frame.

This publish/subscribe pattern is the primary mechanism by which Lua scripts
react to engine happenings (a timer elapsing, a target being reached, and
future event types) without the engine needing to know anything about story
logic. See [EVENTS.md](EVENTS.md) for the full frame-by-frame walkthrough,
the Lua-side `coroutine.wrap` idiom, and known sharp edges.

## Rendering pipeline

`engine::graphics::GraphicsDevice` wraps the low-level OpenGL/GLFW state.
`engine::graphics::SpriteBatch<TextureContainer>`
(`engine/include/graphics/spritebatch.hpp`) batches 2D sprite draw calls; it
is templated on a texture container so a single batched draw call can sample
multiple simultaneously-bound textures per sprite (tilegame uses this to draw
a base color texture and a separate luminosity/glow texture together, see
`engine::Texture2DContainer<2>` usage in `systems::Player`/`systems::Render`).

On top of that, `engine::graphics::PostProcessor`
(`engine/include/graphics/postprocessor.hpp`) renders the scene into an
off-screen framebuffer and then applies a **chain** of
`PostProcessingEffect`s, each an independent shader pass reading from one or
more input textures (often the previous effect's color attachment) and
writing to its own color attachment(s). `WorldScene` wires up two chained
effects to implement day/night lighting:

```
scene color ─▶ [daytime effect]  ──┐
                                    ├─▶ [blend effect] ─▶ final image
scene bright ("luminosity") color ─┘
```

`systems::Daytime` (`tilegame/src/systems/daytime.cpp`) owns a simple
timeline of time-of-day tint colors and continuously updates the shader
uniform the daytime effect uses to tint the scene, giving the lighting cycle
mentioned in the project's design goals.

## Tilemaps

`engine::tilemap::TileMap`/`TileLayer`/`Tileset`/`Tile`/`TileObject`
(`engine/include/tilemap/`) model maps authored in the
[Tiled](https://www.mapeditor.org/) map editor (`tilegame/content/maps`,
`.tiled-project`/`.tiled-session`), parsed via the vendored `tileson`
library. `systems::Map` loads these into entt entities/components
(`components::TileLayer`, `components::TileMap`, `components::Tileset`) that
`systems::Render` and `systems::CollisionDetection` consume — collision
shapes are attached per-tile so the same sweep-based AABB/circle collision
routines used for entity-vs-entity collision also work for entity-vs-tilemap
collision.

## Lua scripting bridge

This is the mechanism that is meant to grow into "author entire games in
Lua." Three pieces cooperate:

1. **`tilegame::SecureLuaState`** (`tilegame/src/secureluastate.hpp`) is a
   sandboxed `sol::state`: it builds a fresh global environment exposing only
   a whitelisted set of base functions and standard libraries
   (`coroutine`, `table`, `string`, `math`, a restricted `debug`, a restricted
   `os`), and replaces `require`/`load` with safe versions (bytecode chunks
   are rejected outright; only an internal `debugger` and `inspect` module
   are `require`-able). The intent is that game/story scripts can be treated
   as data — safe to load and run without trusting their author with the full
   power of Lua or the host filesystem.

2. **`tilegame::systems::Script`** (`tilegame/src/systems/script.cpp`) owns
   the `SecureLuaState` and is the single place that wires the ECS into Lua:
   - It registers `glm::vec2` and `entt::entity` as Lua usertypes, and
     exposes the raw `entt::registry` to Lua as `_registry` (via
     `vendor/entt_sol`, a small bridge library that lets Lua call
     `_registry:create()`, `_registry:emplace(entity, component)`, etc. using
     runtime type info).
   - It calls `register_component(sol::state&)` on every component type that
     should be visible to scripts. Each such component implements this static
     method itself (see `components::Target::register_component` for the
     canonical example), registering an `entt_sol` meta type plus a
     `sol::usertype` — by convention named with a leading underscore in Lua
     (C++ `Target` → Lua `_Target`).
   - It registers event types via `register_event_type<EventType>()`, which
     wires up the Lua-callable `_add_event_listener(_EventType, callback,
     sourceEntity)` used to subscribe a Lua function to an engine event (see
     Events, above).
   - Every frame, it looks for entities carrying a `components::ScriptLoader`
     (a component holding a file path), loads and runs that Lua file exactly
     once (passing a table `{entity = <this entity>}` as the script's sole
     argument), and stores the table the script returns as a
     `components::LuaTable` component on the entity. The `ScriptLoader`
     component is then removed, so each script runs once, at entity-creation
     time, to set up its own state and event subscriptions.

3. **Game/story scripts** (`tilegame/content/scripts/*.lua`) are plain Lua
   files following one recurring idiom: build a small dispatch table keyed by
   `EVENT_TYPE`, wrap each event handler in `coroutine.wrap(...)`, subscribe
   via `_add_event_listener`, and let the coroutine's own control flow (loops,
   `coroutine.yield()`) encode a per-entity state machine — e.g. an NPC that
   patrols between waypoints by re-emplacing a `_Target` component each time
   it receives a `TargetReachedEvent`, or a character that prints a line every
   time a repeating `_Timer` fires. This is currently the whole "story
   scripting language": entt component mutation plus coroutine-driven event
   handling, with no dedicated dialogue/quest DSL yet.

### Adding a new Lua-facing feature

The established pattern for extending the engine with something scripts can
use is:

1. Add a new component struct under `tilegame/src/components/` (plus a
   system under `tilegame/src/systems/` if it needs per-frame behavior).
2. Give the component a static `register_component(sol::state&)` following
   `components::Target` as a template.
3. Call that registration from `systems::Script::register_api()`
   (`tilegame/src/systems/script.cpp`).
4. If it's an event type, also call `register_event_type<T>()` there.

## Build system

CMake, C++20, with three top-level targets wired from the root
`CMakeLists.txt`: `glad` (vendored OpenGL loader), `engine` (static library),
and `tilegame` (executable, links `engine` and system `lua`). Headers/sources
are globbed per-directory in `engine/CMakeLists.txt` and
`tilegame/CMakeLists.txt`. `tilegame`'s build also copies (and symlinks, for
debug builds) `tilegame/content/` next to the built executable so relative
resource paths used in scripts and resource-loading calls resolve correctly
at runtime. External prerequisites (GLFW, glm, EnTT, Lua) are expected to be
installed system-wide; smaller dependencies are vendored directly under
`vendor/`.

## Current status / known gaps

- Only a small, fixed set of event types exist today (`TimerEvent`,
  `TargetReachedEvent`); there is no dialogue, inventory, quest-state, or
  save/load system yet — these would each likely follow the
  component+system+Lua-registration pattern above.
- Script loading happens inside `Script::update()`, with a `// TODO` in the
  source noting it should eventually happen somewhere else (e.g. at map/entity
  load time rather than piggybacking on the update loop).
- The battle system is explicitly deferred; nothing in the current ECS models
  turn-based or real-time combat.
