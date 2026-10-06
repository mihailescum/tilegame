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
-- _set_weather_precipitation(emitter, spawn_area), which starts a new precipitation effect - it
-- has no concept of "rain" or "snow" beyond whatever the emitter and area look like. Any effect
-- that was already running stops spawning but lets its particles finish falling, so switching
-- e.g. rain to snow overlaps briefly instead of popping. _set_weather_precipitation() with no
-- arguments stops the running effect the same way without starting a new one; once its last
-- particle has expired, systems::Weather removes it from the registry entirely.
--
-- Each precipitation entity is Pin'd to the camera (itself pinned to player 1, see
-- systems::Weather), so `spawn_area` is a rectangle relative to the view, not world space.
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

local RAIN = _ParticleEmitter(
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
local RAIN_AREA = _Rectangle(vec2(-800.0, -700.0), vec2(1600.0, 250.0))

local SNOW = _ParticleEmitter(
    150,
    vec2(0.0, 1.0), math.pi / 6.0,    -- wider spread for drifting motion
    40.0, 90.0,
    4.0, 6.0,
    0.15, 0.3,
    _Color(1.0, 1.0, 1.0, 0.85),
    _Rectangle(vec2(0.0, 0.0), vec2(64.0, 64.0))) -- soft round sprite
-- Snow falls much slower, so its band sits closer to the top edge - it doesn't need as much
-- room above the screen to visibly enter from the top.
local SNOW_AREA = _Rectangle(vec2(-800.0, -550.0), vec2(1600.0, 1000.0))

-- Thunderstorm: denser, faster, heavier and more opaque rain than RAIN, with a slightly wider
-- spread so it looks more chaotic. Still falls straight down - particles aren't rotated to
-- match their direction, so slanted rain would look like vertical streaks sliding sideways.
local STORM_RAIN = _ParticleEmitter(
    700,                              -- rate: over twice RAIN's
    vec2(0.0, 1.0), math.pi / 20.0,   -- spread: a bit wider than RAIN
    900.0, 1300.0,                    -- speed range
    0.9, 1.3,                         -- lifetime range, in seconds
    0.4, 0.8,                         -- scale range
    _Color(0.6, 0.7, 0.9, 0.75),      -- darker blue, more opaque than RAIN
    _Rectangle(vec2(128.0, 0.0), vec2(64.0, 64.0))) -- tapering streak sprite
-- Same band as RAIN_AREA: speed (900-1300) x lifetime (0.9-1.3s) covers 810-1690 units, so it
-- falls through the visible height from there too.
local STORM_RAIN_AREA = _Rectangle(vec2(-800.0, -700.0), vec2(1600.0, 250.0))
-- Dark, slightly blue overcast the storm fades the scene towards.
local STORM_TINT = _Color(0.45, 0.45, 0.55, 1.0)
local CLEAR_TINT = _Color(1.0, 1.0, 1.0, 1.0)
-- Seconds to fade the tint in when a storm starts / back out when the weather ends.
local TINT_FADE_DURATION = 3.0
-- Short rumble played on every lightning strike during a thunderstorm. Runs on top of any
-- other shake (e.g. a scripted earthquake) rather than replacing it.
local THUNDER_SHAKE = {
    horizontal = { displacement_speed = 300, offset = 6, duration = 0.4 },
    vertical = { displacement_speed = 300, offset = 4, duration = 0.4 },
}

-- Listener entity for the thunderstorm's _LightningEvent, or nil while no storm is running.
local thunder_listener = nil

-- Example, called from any script to bring in a storm and clear it later:
--   _set_weather_tint(_Color(0.55, 0.55, 0.65, 1.0), 4.0) -- darken over 4 seconds
--   _set_weather_precipitation(RAIN, RAIN_AREA)
--   _set_lightning(5.0, 15.0, 0.2) -- a strike every 5-15 seconds, flash decaying over 0.2s (see systems::Lightning)
--   _set_weather_tint(_Color(1.0, 1.0, 1.0, 1.0), 4.0)     -- clear back to normal over 4 seconds
--   _set_weather_precipitation()
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

local function start_rain()
    _set_weather_precipitation(RAIN, RAIN_AREA)
end

local function start_snow()
    _set_weather_precipitation(SNOW, SNOW_AREA)
end

local function stop_thunder()
    if thunder_listener then
        _remove_event_listener(thunder_listener)
        thunder_listener = nil
    end
end

local function start_thunderstorm()
    _set_weather_tint(STORM_TINT, TINT_FADE_DURATION)
    _set_weather_precipitation(STORM_RAIN, STORM_RAIN_AREA)
    _set_lightning(3.0, 4.0, 0.3) -- a strike every 3-4 seconds, flash decaying over 0.3s

    stop_thunder() -- don't stack a second rumble listener if a storm is already running
    thunder_listener = _add_event_listener(_LightningEvent, function(event_type, event, source)
        _shake_camera(THUNDER_SHAKE)
    end)
end

-- Ends any of the above, including a thunderstorm's lightning, thunder and darkened tint.
local function end_weather()
    _set_weather_precipitation()
    _clear_lightning()
    stop_thunder()
    _set_weather_tint(CLEAR_TINT, TINT_FADE_DURATION)
end

local weather = {...}
weather.start_rain = start_rain
weather.start_snow = start_snow
weather.start_thunderstorm = start_thunderstorm
weather.end_weather = end_weather
return weather
