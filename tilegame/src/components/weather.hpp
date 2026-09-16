#pragma once

#include "engine.hpp"

#include "components/particleemitter.hpp"

namespace tilegame::components
{
    // ---- Command: emplaced on a fresh entity by the _set_weather_tint Lua binding (see
    // Script::set_weather_tint()), consumed and destroyed by systems::Weather the next time it
    // updates. Unlike Daytime's keyframe list, weather has no fixed schedule - it only ever
    // fades from whatever tint it is currently showing towards `target_tint`, over
    // `fade_duration` seconds (e.g. darkening the scene as rain moves in, or clearing back to
    // _Color(1, 1, 1, 1) once it passes). Composed with the day/night tint multiplicatively in
    // content/shaders/daytime.frag, so the two never fight over the same uniform.
    struct SetWeatherTintEvent
    {
        engine::Color target_tint;
        float fade_duration;
    };

    // ---- Command: emplaced on a fresh entity by the _set_weather_precipitation Lua binding
    // (see Script::set_weather_precipitation()), consumed and destroyed by systems::Weather the
    // next time it updates. Replaces the precipitation entity's ParticleEmitter and Shape
    // wholesale with `emitter`/`spawn_area` - built entirely by the calling script (see
    // content/scripts/weather.lua) - and clears its Inactive tag. `spawn_area` is relative to
    // the precipitation entity's Transform, which tracks player 1 (see
    // systems::Weather::create_precipitation_entity()) rather than the camera, so a script
    // authoring e.g. a rain preset should place it as a band above where the player usually is
    // on screen, wide/tall enough that rain visibly falls into view instead of popping in
    // everywhere across the screen at once. The precipitation entity itself always exists
    // (created once by systems::Weather::load_content(), its handle kept in the registry
    // context - see weather.cpp); systems::Weather itself has no notion of "rain" or "snow", it
    // only knows how to run one emitter, however Lua chooses to configure and place it.
    struct SetWeatherPrecipitationEvent
    {
        ParticleEmitter emitter;
        engine::Rectangle spawn_area;
    };

    // ---- Command: emplaced on a fresh entity by the _clear_weather_precipitation Lua binding
    // (see Script::clear_weather_precipitation()), consumed and destroyed by systems::Weather
    // the next time it updates. Re-tags the (always-existing) precipitation entity Inactive,
    // rather than destroying anything.
    struct ClearWeatherPrecipitationEvent
    {
    };
} // namespace tilegame::components
