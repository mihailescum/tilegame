# The Event System

This document explains how Tilegame's native ↔ Lua event system works, end to
end, with the exact frame timing and a few sharp edges that are easy to trip
over. See [ARCHITECTURE.md](ARCHITECTURE.md) for how this fits into the rest
of the engine.

## TL;DR

- An **event** is a normal entt component (e.g. `TimerEvent`) that a system
  attaches to an entity for exactly one frame to say "this happened, here".
- A **listener** is also a normal entt component (`EventListener<T>`) that
  wraps a callback. Listeners live on their own dedicated entities, not on
  the entity the event happens to.
- `System::raise_events<T>()` is the pub/sub step: once per frame, it finds
  every entity carrying event component `T` and calls every listener for
  `T`, passing `(EVENT_TYPE, event_data, source_entity)`.
- Event components are erased again in each system's `end_update()`, so they
  are only ever visible for the frame they were raised in.
- Lua never touches `raise_events` directly. `systems::Script` is the only
  C++ code that creates `EventListener<T>` entities, and it does so on
  behalf of Lua's `_add_event_listener(...)` call. Scripts almost always wrap
  their handler in `coroutine.wrap(...)` so the handler can be written as a
  loop instead of a one-shot callback.

Put differently: this is a small, homegrown observer pattern, implemented
twice — once generically in C++ (`raise_events`/`EventListener<T>`), and once
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
│    EventListener<T> component + System::raise_events<T>()      │
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
   exposing `EVENT_TYPE` as a Lua field on that table.

2. **Raising.** Each frame, `systems::Timer::update()` decrements every
   `Timer` component; when one expires it does
   `_registry.emplace<TimerEvent>(entity, ...)` on the *source* entity (the
   one that owns the `Timer`), then calls `raise_events<TimerEvent>()`.

3. **Dispatch.** `System::raise_events<TimerEvent>()`
   (`systems/system.hpp`) does a double loop: for every entity with a
   `TimerEvent` component, for every entity with an
   `EventListener<TimerEvent>` component, call
   `listener_component(TimerEvent::EVENT_TYPE, event, source_entity)`.
   `EventListener<T>::operator()` checks the listener's `target_entity`
   filter (see below) and, if it passes, invokes the wrapped
   `std::function`.

4. **The listener's callback**, for any event type registered through
   `systems::Script`, is a `sol::function` — a Lua callable — stored inside
   the `std::function`. Calling it runs Lua code with the event type string,
   the event data (as a userdata wrapping the C++ struct), and the source
   entity.

5. **Cleanup.** `systems::Timer::end_update()` does
   `_registry.clear<TimerEvent>()`. Since `WorldScene::end_update()` runs
   after every system's `update()` for the frame, the `TimerEvent` component
   only exists for the remainder of that one frame — any code checking for
   it next frame will find nothing.

The listener entity created in step 3/4, notably, is **not** cleaned up this
way — see "Listener entities are permanent" below.

## Frame lifecycle

`WorldScene::update()` (`worldscene/worldscene.cpp`) calls systems in a fixed
order every frame; the ones relevant to events happen in this sequence:

```
update():
  Timer.update()              → may emplace<TimerEvent> + raise_events<TimerEvent>()
  Particle.update()
  Player.update()
  Script.update()             → runs newly-loaded scripts, which may call
                                 _add_event_listener (creates listener entities)
  MovementController.update()
  CollisionDetection.update()
  Movement.update()           → may emplace<TargetReachedEvent>/<MapEnteredEvent>/
                                 <MapLeftEvent> + raise_events<...>() for each
  Animation.update()
  Pin.update()
  Camera.update()

end_update():
  Timer.end_update()              → clear<TimerEvent>()
  MovementController.end_update()
  Movement.end_update()           → clear<TargetReachedEvent/MapLeftEvent/MapEnteredEvent>()
```

The important, easy-to-miss consequence: **`Script.update()` runs after
`Timer.update()` but before `Movement.update()`.** A script that is loaded
this frame and calls `_add_event_listener(_TimerEvent, ...)` will *not* see
a `TimerEvent` raised earlier in that same frame (the dispatch loop already
ran before the listener existed) — it only starts receiving `TimerEvent`s
from the *next* time the timer fires. But that same script's
`_add_event_listener(_TargetReachedEvent, ...)` or
`_MapEnteredEvent`/`_MapLeftEvent` listeners *can* fire on their very first
frame, because `Movement` (which raises those three) hasn't run yet when
`Script.update()` executes.

## Existing event types

| Event | `EVENT_TYPE` | Raised by | Raised when | Payload | Cleared by |
|---|---|---|---|---|---|
| `TimerEvent` | `"TIMER_EVENT"` | `systems::Timer` | a `Timer` component's `time_left` reaches 0 | `duration` (float), `repeated` (bool) | `Timer::end_update()` |
| `TargetReachedEvent` | `"TARGET_REACHED_EVENT"` | `systems::Movement` | an entity with `Target` arrives at its destination (also removes `Target`/`Movement`/`Speed`) | `target` (vec2) | `Movement::end_update()` |
| `MapEnteredEvent` | `"MAP_ENTERED_EVENT"` | `systems::Movement` | an entity's computed `CurrentMap` changes to a new, non-empty map | `map_name` (string) | `Movement::end_update()` |
| `MapLeftEvent` | `"MAP_LEFT_EVENT"` | `systems::Movement` | an entity's `CurrentMap` changes away from a previous map | `map_name` (string, the *old* map) | `Movement::end_update()` |

All four are registered for Lua in `systems::Script::register_api()`
(`systems/script.cpp`) via `register_event_type<T>()`, and all four have
their component registered via `T::register_component()` so their payload
fields are readable from Lua.

## Filtering by source entity

`EventListener<T>` (`components/event.hpp`) carries an optional
`target_entity`. If it's `entt::null` (the default), the listener fires for
an event from *any* source entity. If it's set, the listener only fires when
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

- **Listener entities are permanent.** Every call to `_add_event_listener`
  (via `Script::register_event_type<T>()`'s lambda in `script.cpp`) does
  `_registry.create()` + `emplace<EventListener<T>>(...)` and never destroys
  that entity. There is no unsubscribe. Listener entities only disappear
  when `Script`'s destructor calls `_registry.clear()` (i.e. the whole scene
  is torn down). A script that re-subscribes every time it runs (e.g. one
  reloaded per entity spawn) will accumulate one listener entity per
  subscription for the lifetime of the scene.
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
- **Events don't carry a queue** — only one `TimerEvent`/etc. component can
  exist per entity per frame (it's a component, entt entities have at most
  one of each component type). If a system could somehow raise the same
  event type twice for the same entity in one frame, the second `emplace`
  would need to be an `emplace_or_replace`/would throw — in practice this
  isn't an issue today since each raising system only ever emplaces once per
  entity per frame, but it's a constraint to keep in mind when adding a new
  event-raising system.

## How to add a new native event type

1. Define the event struct next to whatever component/system produces it
   (e.g. `components/target.hpp` for `TargetReachedEvent`), following the
   existing structs: a `static const std::string EVENT_TYPE`, payload
   fields, and a `static void register_component(sol::state&)`.
2. In `register_component`, call `entt_sol::register_meta_component<T>()`
   and `lua.new_usertype<T>("_YourEvent", ..., "EVENT_TYPE",
   sol::var(T::EVENT_TYPE.c_str()), ...)`.
3. In the system that should raise it: `_registry.emplace<T>(source_entity,
   ...)` when the condition happens, then call `raise_events<T>()` (inherited
   from `System`), then clear it again in that system's `end_update()` via
   `_registry.clear<T>()`.
4. In `systems::Script::register_api()` (`systems/script.cpp`): add
   `T::register_component(_lua())` and `register_event_type<T>()`.
5. In Lua, subscribe with `_add_event_listener(_YourEvent, callback[,
   source])`, typically wrapped in `coroutine.wrap(...)` following the
   existing scripts.

## File map

| File | Role |
|---|---|
| `tilegame/src/components/event.hpp` | `EventListener<T>` template |
| `tilegame/src/systems/system.hpp` | `System::raise_events<T>()` |
| `tilegame/src/systems/script.hpp`/`.cpp` | Lua bridge: `register_event_type<T>()`, `_add_event_listener` |
| `tilegame/src/components/timer.hpp`/`.cpp` | `Timer`, `TimerEvent` |
| `tilegame/src/components/target.hpp`/`.cpp` | `Target`, `TargetReachedEvent` |
| `tilegame/src/components/currentmap.hpp`/`.cpp` | `CurrentMap`, `MapEnteredEvent`, `MapLeftEvent` |
| `tilegame/src/systems/timer.cpp` | Raises/clears `TimerEvent` |
| `tilegame/src/systems/movement.cpp` | Raises/clears `TargetReachedEvent`, `MapEnteredEvent`, `MapLeftEvent` |
| `tilegame/content/scripts/man1.lua`, `soldier1.lua` | Example Lua-side listener usage |
