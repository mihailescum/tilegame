-- Global weather overlay configuration.
--
-- Run once at startup by systems::Script (see Script::initialize()). Unlike daytime.lua,
-- weather has no fixed schedule of its own - it starts clear, and any other script can call
-- _set_weather_tint(color, fade_duration) at any time (e.g. from a TimerEvent handler, or a story
-- trigger) to fade the whole scene towards `color` over `fade_duration` seconds. The result is
-- multiplied with the day/night tint, so it darkens whatever the current time of day already
-- looks like rather than overriding it.
--
-- Precipitation works differently: instead of picking from a fixed set of presets baked into
-- C++, the caller builds the whole _ParticleEmitter itself (rate, spread, speed, lifetime,
-- scale, color, and which region of content/textures/particles.png to sample) plus a
-- _Rectangle spawn area, and hands both to systems::Weather via
-- _set_weather_precipitation(emitter, spawn_area), which creates or reconfigures the one active
-- precipitation effect - it has no concept of "rain" or "snow" beyond whatever the emitter and
-- area look like. _clear_weather_precipitation() stops it and removes it from the registry
-- entirely, so nothing precipitation-related lingers while the weather is clear.
--
-- The precipitation entity is Pin'd to player 1 (see systems::Weather), not the camera, so
-- `spawn_area` is a rectangle relative to wherever the player currently is, not screen space.
-- Player 1 is roughly centered on screen (systems::Camera also pins the camera to it), so a
-- rain/snow area should be a band positioned just above the top edge of the screen and wide
-- enough to span it, letting particles fall into view from above rather than spawning
-- uniformly across the whole screen every frame - the latter piles up more particles towards
-- the bottom as they fall, since every row is fed by everything spawned above it, not just its
-- own row. The numbers below assume the 1200x900 window main.cpp opens and Camera's default
-- scale of 1.0 (so 1 world unit = 1 pixel); retune them if either changes.
--
-- Below are rain/snow presets any script can reuse or override; feel free to add more (e.g.
-- ash, falling leaves) the same way.

RAIN = _ParticleEmitter(
    300,                              -- rate: particles/second
    vec2(0.0, 1.0), math.pi / 32.0,   -- spread: mostly straight down, slight variance
    600.0, 900.0,                     -- speed range
    1.0, 1.5,                         -- lifetime range, in seconds
    0.3, 0.6,                         -- scale range
    _Color(0.7, 0.8, 1.0, 0.6),       -- pale blue, semi-transparent
    _Rectangle(vec2(128.0, 0.0), vec2(64.0, 64.0))) -- tapering streak sprite
-- A band starting 700 units above the player and 250 tall, i.e. well above the visible top
-- edge (~450 units above player at this window size) - rain speed (600-900) x lifetime (1-1.5s)
-- covers 600-1350 units, enough to fall all the way through the visible height from there.
RAIN_AREA = _Rectangle(vec2(-800.0, -700.0), vec2(1600.0, 250.0))

SNOW = _ParticleEmitter(
    150,
    vec2(0.0, 1.0), math.pi / 6.0,    -- wider spread for drifting motion
    40.0, 90.0,
    4.0, 6.0,
    0.15, 0.3,
    _Color(1.0, 1.0, 1.0, 0.85),
    _Rectangle(vec2(0.0, 0.0), vec2(64.0, 64.0))) -- soft round sprite
-- Snow falls much slower, so its band sits closer to the top edge - it doesn't need as much
-- room above the screen to visibly enter from the top.
SNOW_AREA = _Rectangle(vec2(-800.0, -550.0), vec2(1600.0, 1000.0))

-- Example, called from any script to bring in a storm and clear it later:
--   _set_weather_tint(_Color(0.55, 0.55, 0.65, 1.0), 4.0) -- darken over 4 seconds
--   _set_weather_precipitation(RAIN, RAIN_AREA)
--   _set_lightning(5.0, 15.0, 0.2) -- a strike every 5-15 seconds, flash decaying over 0.2s (see systems::Lightning)
--   _set_weather_tint(_Color(1.0, 1.0, 1.0, 1.0), 4.0)     -- clear back to normal over 4 seconds
--   _clear_weather_precipitation()
--   _clear_lightning()
--
-- A strike's screen flash and its _LightningEvent (subscribable via _add_event_listener, e.g.
-- to play a thunder sound a beat after the flash) fire regardless of whether it's raining -
-- systems::Lightning knows nothing about systems::Weather.
--
-- A lighter drizzle or flurry is just a copy of the preset with a lower rate, e.g.:
--   local drizzle = _ParticleEmitter(RAIN.rate * 0.3, RAIN.spread_direction, RAIN.spread_angle,
--       RAIN.speed.x, RAIN.speed.y, RAIN.lifetime.x, RAIN.lifetime.y,
--       RAIN.scale.x, RAIN.scale.y, RAIN.color, RAIN.source_rect)

_set_weather_precipitation(SNOW, SNOW_AREA)
_shake_camera_horizontal(500, 20, 5) -- chase within +-20 units at up to 500 units/sec, for 5 seconds
_set_lightning(1, 2, 0.2)