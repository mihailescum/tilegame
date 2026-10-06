#pragma once

#include <string>

#include "engine.hpp"

#include "components/particleemitter.hpp"

namespace tilegame::components
{
    // ---- Raised via System::raise() by the _set_weather_tint Lua binding (see
    // Script::set_weather_tint()), immediately delivered to systems::Weather's
    // EventListener<SetWeatherTintEvent>. Unlike Daytime's keyframe list, weather has no fixed
    // schedule - it only ever fades from whatever tint it is currently showing towards
    // `target_tint`, over `fade_duration` seconds (e.g. darkening the scene as rain moves in, or
    // clearing back to _Color(1, 1, 1, 1) once it passes). Composed with the day/night tint
    // multiplicatively in content/shaders/daytime.frag, so the two never fight over the same
    // uniform. Not exposed to Lua as a usertype - nothing outside systems::Weather subscribes to
    // it today - but carries EVENT_TYPE like any other event since System::raise_event() needs
    // it regardless.
    struct SetWeatherTintEvent
    {
        inline static const std::string EVENT_TYPE = "SET_WEATHER_TINT_EVENT";

        engine::Color target_tint;
        float fade_duration;
    };

    // Tag marking an entity as one precipitation effect created by systems::Weather (see
    // Weather::create_precipitation_entity()). Several can exist at once: only the newest still
    // carries a ParticleEmitter and spawns particles; older ones have had theirs removed and are
    // just letting their already-falling particles finish (systems::Particle keeps ageing and
    // systems::Render keeps drawing a ParticlePool without a ParticleEmitter), until
    // systems::Weather destroys them once their pool is empty.
    struct Precipitation
    {
    };

    // ---- Raised via System::raise() by the _set_weather_precipitation(emitter, spawn_area) Lua
    // binding (see Script::set_weather_precipitation()), immediately delivered to
    // systems::Weather's EventListener<SetWeatherPrecipitationEvent> (registered in
    // Weather::load_content()). Stops whatever precipitation is currently emitting (its
    // particles keep falling until they expire, see Precipitation) and starts a new one on a
    // fresh entity with `emitter`/`spawn_area` - both built entirely by the calling script (see
    // content/scripts/weather.lua) - so switching e.g. rain to snow cross-fades instead of
    // popping. `spawn_area` is relative to the new entity's Transform, which is Pin'd to the
    // camera (see systems::Weather::create_precipitation_entity()), so a script authoring e.g.
    // a rain preset should place it as a band above the top edge of the screen, wide/tall
    // enough that rain visibly falls into view instead of popping in everywhere across the
    // screen at once. systems::Weather itself has no notion of "rain" or "snow", it only knows
    // how to run emitters, however Lua chooses to configure and place them.
    struct SetWeatherPrecipitationEvent
    {
        inline static const std::string EVENT_TYPE = "SET_WEATHER_PRECIPITATION_EVENT";

        ParticleEmitter emitter;
        engine::Rectangle spawn_area;
    };

    // ---- Raised via System::raise() by the parameterless _set_weather_precipitation() Lua
    // binding (see Script::set_weather_precipitation()), immediately delivered to
    // systems::Weather's EventListener<ClearWeatherPrecipitationEvent>. Stops whatever
    // precipitation is currently emitting; its particles already in the air keep falling until
    // they expire, after which systems::Weather destroys the entity (see Precipitation).
    struct ClearWeatherPrecipitationEvent
    {
        inline static const std::string EVENT_TYPE = "CLEAR_WEATHER_PRECIPITATION_EVENT";
    };
} // namespace tilegame::components
