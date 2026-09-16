# The Event System

This document explains how Tilegame's native ↔ Lua event system works, end to
end, with the exact frame timing and a few sharp edges that are easy to trip
over. See [ARCHITECTURE.md](ARCHITECTURE.md) for how this fits into the rest
of the engine.

## TL;DR

- An **event** (e.g. `TimerEvent`, `ShowMessageEvent`) is a plain value type
  — never an entt component, never stored in the registry. Raising one
  constructs it locally and hands it straight to whoever's listening.
- A **listener** is a normal entt component (`EventListener<T>`) that wraps a
  callback. Listeners live on their own dedicated entities, not on the
  entity the event is about.
- `System::raise_event<T>(source, args...)` is the raise-and-deliver step,
  all in one synchronous call: build `T{args...}`, find every entity
  carrying `EventListener<T>`, call each one with
  `(EVENT_TYPE, event, source)`. `System::raise<T>(args...)` is a
  convenience for when the event isn't about any particular entity —
  `raise_event<T>(entt::null, args...)`.
- There's nothing to clean up afterwards. The event value only exists for
  the duration of the call; no system needs an `end_update()` to erase it.
- Lua never touches `raise_event`/`raise` directly. `systems::Script` is
  the only C++ code that creates `EventListener<T>` entities, and it does so
  on behalf of Lua's `_add_event_listener(...)` call. Scripts almost always
  wrap their handler in `coroutine.wrap(...)` so the handler can be written
  as a loop instead of a one-shot callback.

Put differently: this is a small, homegrown observer pattern, implemented
twice — once generically in C++ (`raise_event`/`EventListener<T>`), and once
again on top of that in Lua (`_add_event_listener`/`coroutine.wrap`). The
"messy" feeling mostly comes from those two layers not being obviously
separated when you're just reading `.lua` files.

## The three layers

```
┌─────────────────────────────────────────────────────────────┐
│ 3. Lua scripts (content/scripts/*.lua)                       │
│    coroutine.wrap(handler)  +  _add_event_listener(...)      │
└───────────────────────────▲───────────────────────────────────┘
                             │ calls into
┌───────────────────────────┴───────────────────────────────────┐
│ 2. systems::Script (systems/script.cpp)                       │
│    register_event_type<T>()  →  Lua-callable                 │
│    _add_event_listener(eventTable, callback[, source])        │
│    creates a new EventListener<T> entity per subscription     │
└───────────────────────────▲───────────────────────────────────┘
                             │ built on
┌───────────────────────────┴───────────────────────────────────┐
│ 1. Native ECS event mechanism (systems/system.hpp,             │
│    components/event.hpp)                                      │
│    EventListener<T> component + System::raise_event<T>()      │
│    Works with plain C++ std::function callbacks; knows          │
│    nothing about Lua.                                          │
└─────────────────────────────────────────────────────────────┘
```

Layer 1 is generic and reusable: any system could raise any event type and
any code (C++ or Lua-wrapped) could listen, without either side knowing the
other exists. Layer 2 is the *specific* bridge that lets Lua create layer-1
listeners. Layer 3 is just a coding convention scripts follow on top of
layer 2 — nothing enforces it structurally.

## Anatomy of one event, step by step

Using `TimerEvent` (`components/timer.hpp`, `systems/timer.cpp`) as the
running example:

1. **Definition.** `TimerEvent` is a plain struct with a `static const
   std::string EVENT_TYPE = "TIMER_EVENT"` and whatever payload data it
   carries (`duration`, `repeated`). It has a `register_component(sol::state&)`
   that registers it as `entt_sol` meta type + Lua usertype `_TimerEvent`,
   exposing `EVENT_TYPE` as a Lua field on that table. It is never stored as
   an entt component — `register_component` only makes it constructible and
   readable from Lua as a plain value.

2. **Raising.** Each frame, `systems::Timer::update()` decrements every
   `Timer` component; when one expires it immediately calls
   `raise_event<TimerEvent>(entity, timer.time_total, timer.repeat)` —
   `entity` is the one that owns the `Timer`.

3. **Dispatch.** `System::raise_event<TimerEvent>(source, args...)`
   (`systems/system.hpp`) builds `const TimerEvent event{args...}` as a
   local value, finds every entity carrying `EventListener<TimerEvent>`, and
   calls `listener_component(TimerEvent::EVENT_TYPE, event, source)` for
   each. `EventListener<T>::operator()` checks the listener's
   `target_entity` filter (see below) and, if it passes, invokes the wrapped
   `std::function`. All of this happens before `raise_event()` returns —
   there is no batching or per-frame collection step.

4. **The listener's callback**, for any event type registered through
   `systems::Script`, is a `sol::function` — a Lua callable — stored inside
   the `std::function`. Calling it runs Lua code with the event type string,
   the event data (as a userdata wrapping the C++ struct, valid only for the
   duration of the call), and the source entity.

5. **Cleanup:** none needed. `event` was a local variable in
   `raise_event()`'s stack frame; once that call returns, it's gone. There
   is no `end_update()` step for `TimerEvent` (or any other event type) to
   erase.

The listener entity, notably, is **not** cleaned up automatically — see
"Listener entities aren't cleaned up automatically" below.

## `System::raise<T>()`: events with no particular source

`TimerEvent` above needs a meaningful *source* entity (the one whose `Timer`
rang), passed explicitly to `raise_event()`. Most events raised from Lua
bindings (`_show_message`, `_set_lightning`, `_stop_player_input`,
`_shake_camera_horizontal`, the `_set_daytime_*`/`_set_weather_*` family, …)
don't — there's nothing meaningful about *which* entity a `ShowMessageEvent`
is "from," only that it happened. For these, `System::raise<T>(args...)` is
shorthand for `raise_event<T>(entt::null, args...)`:

```cpp
template <class Event, class EventListener = components::EventListener<Event>, class... Args>
void raise(Args &&...args) const
{
    raise_event<Event, EventListener>(entt::null, std::forward<Args>(args)...);
}
```

`Script::show_message()`, `Lightning::strike()` (which raises
`LightningEvent`), and the rest of the `_set_X`/`_shake_camera_X` family all
use `raise<T>()`.

This replaced an older split where events raised from Lua bindings were
called "commands": a plain component emplaced on a throwaway entity, picked
up and destroyed by exactly one hardcoded system's own
`_registry.view<T>().each()` loop on its *next* `update()`, never going
through `raise_event`/`EventListener<T>` at all. That worked only as long as
exactly one system ever cared about a given command type — nothing enforced
it, so if a second system had ever wanted to react to the same command,
whichever system's `update()` ran first that frame would silently consume
and destroy it before the second ever saw it. Routing every event —
including former "commands" like `SetLightningEvent`/`ShowMessageEvent`/
`StopPlayerInputEvent` — through the same `raise_event()`/`EventListener<T>`
mechanism `TimerEvent`/`LightningEvent` already used means any number of
systems (or Lua scripts, if the type is ever registered via
`register_event_type<T>()`) can subscribe to the same event without racing,
since there's no shared, destroyable state for two subscribers to fight
over — each is just called directly, synchronously, once per raise.

Every event type — even ones with no Lua usertype registered, and even the
near-empty ones like `ClearLightningEvent` — still needs a
`static const std::string EVENT_TYPE`, because `raise_event<T>()`
references `Event::EVENT_TYPE` unconditionally when dispatching, regardless
of who's actually listening.

## Frame lifecycle

`WorldScene::update()` (`worldscene/worldscene.cpp`) calls systems in a fixed
order every frame; the ones relevant to events happen in this sequence:

```
update():
  Timer.update()              → may raise_event<TimerEvent>(entity, ...) immediately
  Particle.update()
  Player.update()
  Script.update()             → runs newly-loaded per-entity scripts, which may call
                                 _add_event_listener (creates listener entities)
  MovementController.update()
  CollisionDetection.update()
  Movement.update()           → may raise_event<TargetReachedEvent/MapEnteredEvent/
                                 MapLeftEvent>(entity, ...) immediately, per entity
  Animation.update()
  Pin.update()
  Camera.update()

  Daytime.update()
  Weather.update()
  Lightning.update()
```

There's no longer an `end_update()` step for any of this — see "Anatomy of
one event" above for why.

The important, easy-to-miss consequence: **`Script.update()` runs after
`Timer.update()` but before `Movement.update()`.** A script that is loaded
this frame and calls `_add_event_listener(_TimerEvent, ...)` will *not* see
a `TimerEvent` raised earlier in that same frame (dispatch already happened
before the listener existed) — it only starts receiving `TimerEvent`s from
the *next* time the timer fires. But that same script's
`_add_event_listener(_TargetReachedEvent, ...)` or
`_MapEnteredEvent`/`_MapLeftEvent` listeners *can* fire on their very first
frame, because `Movement` (which raises those three) hasn't run yet when
`Script.update()` executes.

Global startup scripts (`content/scripts/daytime.lua`, `weather.lua`) are a
separate case — see the last "Sharp edges" bullet below.

## Existing native event types

| Event | `EVENT_TYPE` | Raised by | Raised when | Payload |
|---|---|---|---|---|
| `TimerEvent` | `"TIMER_EVENT"` | `systems::Timer` | a `Timer` component's `time_left` reaches 0 | `duration` (float), `repeated` (bool) |
| `TargetReachedEvent` | `"TARGET_REACHED_EVENT"` | `systems::Movement` | an entity with `Target` arrives at its destination (also removes `Target`/`Movement`/`Speed`) | `target` (vec2) |
| `MapEnteredEvent` | `"MAP_ENTERED_EVENT"` | `systems::Movement` | an entity's computed `CurrentMap` changes to a new, non-empty map | `map_name` (string) |
| `MapLeftEvent` | `"MAP_LEFT_EVENT"` | `systems::Movement` | an entity's `CurrentMap` changes away from a previous map | `map_name` (string, the *old* map) |
| `LightningEvent` | `"LIGHTNING_EVENT"` | `systems::Lightning` | a scheduled strike's random interval elapses | none (empty) |
| `MessageOpenedEvent` | `"MESSAGE_OPENED_EVENT"` | `systems::MessageBox` | a new dialog message becomes active | none (empty) |
| `MessageClosedEvent` | `"MESSAGE_CLOSED_EVENT"` | `systems::MessageBox` | the current dialog message is fully dismissed | `selected_option` (string, empty if the message had no options) |

All of these are registered for Lua in `systems::Script::register_api()`
(`systems/script.cpp`) via `register_event_type<T>()`, and all have their
component registered via `T::register_component()` so their payload fields
are readable from Lua. There are several more event types used only
natively (`ShowMessageEvent`, `SetLightningEvent`/`ClearLightningEvent`, the
`SetDaytime*`/`SetWeather*` family, `ShakeCameraHorizontalEvent`/
`ShakeCameraVerticalEvent`, `StopPlayerInputEvent`/`ResumePlayerInputEvent`)
— these still need `EVENT_TYPE` (see above) but have no Lua usertype, since
nothing outside their one owning system subscribes to them today.

## Filtering by source entity

`EventListener<T>` (`components/event.hpp`) carries an optional
`target_entity`. If it's `entt::null` (the default), the listener fires for
an event from *any* source. If it's set, the listener only fires when
`source == target_entity` — this is what lets a script subscribe to "my
timer" specifically instead of every `Timer` in the game.

From Lua this is the third, optional argument to `_add_event_listener`:

```lua
-- fires for TimerEvents from *any* entity
_add_event_listener(_TimerEvent, callback)

-- fires only for TimerEvents raised on `timer1`
_add_event_listener(_TimerEvent, callback, timer1)
```

## The Lua-side idiom: `coroutine.wrap` dispatch

Scripts (see `content/scripts/man1.lua`, `content/scripts/soldier1.lua`)
consistently follow this shape:

```lua
local function handle_timer_event(type, event, source)
    while true do
        -- do something once per event
        coroutine.yield()
    end
end

_add_event_listener(_TimerEvent, coroutine.wrap(handle_timer_event), timer1)
```

`coroutine.wrap(handler)` produces a plain function. The **first** time that
function is called (i.e. the first time this event fires), it starts the
coroutine, binding `type, event, source` as normal function parameters and
running until the first `coroutine.yield()`. Every **subsequent** call
*resumes* the coroutine — the arguments passed to that call become the
*return values* of the `coroutine.yield()` expression inside the handler,
not new function parameters. This is why the pattern reads like an infinite
loop: each iteration is one call to the wrapped function, i.e. one delivered
event.

### Gotcha: only some scripts re-capture the resumed values

Whether you need to capture `coroutine.yield()`'s return value depends on
whether the handler body uses the event data after the first firing:

```lua
-- man1.lua: never looks at event/source again, so ignoring the
-- yield return value is fine.
local function handle_timer_event(event_type, event, source)
    while true do
        print("Hello man!")
        coroutine.yield()
    end
end
```

```lua
-- soldier1.lua: DOES try to re-read event/source on later firings...
local function handle_timer_event(event_type, event, source)
    local count = 1
    while true do
        print("Soldier! Count:", count)
        count = count + 1
        event_type, event, source = coroutine.yield()   -- see below
    end
end
```

## Sharp edges / known limitations

- **Listener entities aren't cleaned up automatically.** `_add_event_listener`
  (via `Script::register_event_type<T>()`'s lambda in `script.cpp`) does
  `_registry.create()` + `emplace<EventListener<T>>(...)` and returns that
  entity to Lua. Pass it to `_remove_event_listener(listener)` to unsubscribe
  (destroys the entity; a no-op if it's already invalid, e.g. `entt::null` or
  already removed). If you don't hold on to the handle and unsubscribe,
  listener entities persist until `Script`'s destructor calls
  `_registry.clear()` (i.e. the whole scene is torn down) - a script that
  re-subscribes every time it runs (e.g. one reloaded per entity spawn)
  without unsubscribing the old listener will still accumulate one listener
  entity per call.
- **`raise_event()`'s listener lookup excludes entities tagged `Inactive`.**
  If an `EventListener<T>` lives on an entity that's currently `Inactive`,
  it won't be called — this is by design (Inactive generally means "skip
  this entity everywhere"), but it's a trap for a "start" event whose whole
  job is to *remove* `Inactive` from an entity (e.g. `SetLightningEvent`,
  `ShakeCameraHorizontalEvent`, `SetWeatherPrecipitationEvent`): if that
  listener lives on the same entity it's meant to activate, it can never
  fire, since the entity is exactly what's being excluded. `systems::Lightning`/
  `Weather`/`Camera` all hit this and fixed it by registering those specific
  listeners on a separate, always-active entity instead (see their
  `control_entity`/`weather_entity` in `load_content()`) while the "stop"
  side and the effect's own per-frame state stay on the entity that actually
  gets tagged `Inactive`.
- **Registration order matters within a frame** — see "Frame lifecycle"
  above. Whether a listener registered this frame can catch an event raised
  this same frame depends entirely on where the raising system sits relative
  to `Script.update()` in `WorldScene::update()`, which is not something a
  script author can see from the `.lua` file alone.
- **No listener-side type safety beyond the `EVENT_TYPE` string.** Lua's
  `_add_event_listener` looks up `event["EVENT_TYPE"]` in a
  `std::unordered_map<std::string, ...>` (`Script::_event_types`); passing an
  unregistered/mistyped event table just makes `_add_event_listener` return
  `false` silently (no error, no crash) — easy to overlook.
- **Raising inside a loop that iterates the same component types a listener
  might structurally add/remove is unsafe.** `Timer::update()`,
  `Movement::check_target_reached()`, and `Movement::update_current_map()`
  all call `raise_event()` immediately, from inside a `view.each()` loop
  over `Timer`/`Target`+`Transform`/`Movement`+`Transform` respectively. A
  listener that only reads or `patch()`es/`emplace_or_replace()`s existing
  components (as every listener does today) is safe; a hypothetical future
  listener that added or removed one of those component *types* on some
  entity from inside the callback could invalidate the enclosing iteration.
  There's no guard against this — it's a constraint to keep in mind when
  writing a new listener for one of these three event types.
- **`raise<T>()`/`raise_event<T>()` only reach listeners that already
  exist — there's no queue.** Raising *is* the delivery, so an event raised
  before its target's `EventListener<T>` is registered is simply lost,
  silently — no error, no replay once the listener shows up later. This is
  why `systems::Script::load_content()` (which runs the global Lua scripts
  `daytime.lua`/`weather.lua`, see `content/scripts/`) is called *last* in
  `WorldScene::load_content()` (`worldscene.cpp`): those scripts immediately
  call things like `_set_weather_precipitation`/`_shake_camera_horizontal`/
  `_set_lightning`, which only have somewhere to go once
  `systems::Weather`/`systems::Camera`/`systems::Lightning` have already
  created the entities and registered the listeners those events target, in
  their own `load_content()`. A system whose `load_content()`/`initialize()`
  runs *after* `Script::load_content()` but that needs to react to something
  a global startup script raises would silently drop that first event.

## How to add a new native event type

1. Define the event struct next to whatever component/system produces it
   (e.g. `components/target.hpp` for `TargetReachedEvent`), following the
   existing structs: a `static const std::string EVENT_TYPE`, payload
   fields, and - only if Lua needs to listen for it - a
   `static void register_component(sol::state&)`.
2. If registering for Lua, in `register_component` call
   `entt_sol::register_meta_component<T>()` and `lua.new_usertype<T>(
   "_YourEvent", ..., "EVENT_TYPE", sol::var(T::EVENT_TYPE.c_str()), ...)`.
3. To raise it: `raise<T>(args...)` (see above) if it isn't about any
   particular entity - the common case for anything raised from a Lua
   binding. Use `raise_event<T>(source, args...)` directly if `source`
   should identify a specific, already-existing entity for listeners to key
   off of (e.g. `TargetReachedEvent` on the entity that arrived). Either
   way, watch out for calling it from inside a `view.each()` loop over
   component types a listener might structurally add/remove (see "Sharp
   edges" above), and for the Inactive-exclusion trap if the listener is
   meant to activate the same entity it lives on.
4. If registering for Lua, in `systems::Script::register_api()`
   (`systems/script.cpp`): add `T::register_component(_lua())` and
   `register_event_type<T>()`.
5. In Lua, subscribe with `_add_event_listener(_YourEvent, callback[,
   source])`, typically wrapped in `coroutine.wrap(...)` following the
   existing scripts.

## File map

| File | Role |
|---|---|
| `tilegame/src/components/event.hpp` | `EventListener<T>` template |
| `tilegame/src/systems/system.hpp` | `System::raise_event<T>(source, args...)`, `System::raise<T>(args...)` |
| `tilegame/src/systems/script.hpp`/`.cpp` | Lua bridge: `register_event_type<T>()`, `_add_event_listener`/`_remove_event_listener`; every `_set_X`/`_show_message`/`_stop_player_input`-style binding calls `raise<T>()` |
| `tilegame/src/components/timer.hpp`/`.cpp` | `Timer`, `TimerEvent` |
| `tilegame/src/components/target.hpp`/`.cpp` | `Target`, `TargetReachedEvent` |
| `tilegame/src/components/currentmap.hpp`/`.cpp` | `CurrentMap`, `MapEnteredEvent`, `MapLeftEvent` |
| `tilegame/src/systems/timer.cpp` | Raises `TimerEvent` |
| `tilegame/src/systems/movement.cpp` | Raises `TargetReachedEvent`, `MapEnteredEvent`, `MapLeftEvent` |
| `tilegame/src/systems/{lightning,daytime,weather,camera,player,messagebox}.cpp` | Each registers native `EventListener<T>`s (in `load_content()`/`initialize()`) for its own one-shot event types, raised via `System::raise<T>()` from `systems::Script` |
| `tilegame/content/scripts/man1.lua`, `soldier1.lua` | Example Lua-side listener usage |
