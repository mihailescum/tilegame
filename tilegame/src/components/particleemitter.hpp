#pragma once

#include <string>

#include <glm/glm.hpp>

#include "sol/sol.hpp"

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Configures how the Particle system spawns particles into an associated ParticlePool:
     * spawn rate, spread cone, and the randomized ranges particles draw their speed, lifetime and
     * scale from. Requires a Shape component on the same entity, which particles spawn within.
     * Exposed to Lua as `_ParticleEmitter`, so e.g. weather presets (rain/snow) can be authored
     * entirely in content scripts (see content/scripts/weather.lua and
     * Script::set_weather_precipitation()).
     */
    struct ParticleEmitter
    {
        // Particles per second
        int rate = 0;
        /// Accumulator tracking fractional particles owed since the last spawn, in seconds.
        float rate_clock = 0.0f;

        // Direction of spread
        glm::vec2 spread_direction = glm::vec2(0.0f);

        // Angle of spread in radiants
        float spread_angle = 0.0f;

        // Min/Max particle speed
        glm::vec2 speed = glm::vec2(0.0f);

        // Min/Max lifetime of particle
        glm::vec2 lifetime = glm::vec2(0.0f);

        // Min/Max scale of particle
        glm::vec2 scale = glm::vec2(0.0f);

        // Tint color
        engine::Color color;

        // Region within the particle texture (see systems::Particle::load_content()) each
        // spawned particle samples its sprite from, e.g. to pick a rain streak vs. a snowflake
        // out of the same shared atlas.
        engine::Rectangle source_rect;

        ParticleEmitter() = default;
        /// speed/lifetime/scale ranges are given as separate min/max pairs, packed into the vec2 fields above.
        ParticleEmitter(int rate,
                        glm::vec2 spread_direction,
                        float spread_angle,
                        float speed_min,
                        float speed_max,
                        float lifetime_min,
                        float lifetime_max,
                        float scale_min,
                        float scale_max,
                        engine::Color color,
                        engine::Rectangle source_rect)
            : rate(rate),
              rate_clock(0.0),
              spread_direction(spread_direction),
              spread_angle(spread_angle),
              speed(speed_min, speed_max),
              lifetime(lifetime_min, lifetime_max),
              scale(scale_min, scale_max),
              color(color),
              source_rect(source_rect)
        {
        }

        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
