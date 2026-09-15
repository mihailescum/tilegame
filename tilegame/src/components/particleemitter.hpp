#pragma once

#include <glm/glm.hpp>

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Configures how the Particle system spawns particles into an associated ParticlePool:
     * spawn rate, spread cone, and the randomized ranges particles draw their speed, lifetime and
     * scale from. Requires a Shape component on the same entity, which particles spawn within.
     */
    struct ParticleEmitter
    {
        // Particles per second
        int rate;
        /// Accumulator tracking fractional particles owed since the last spawn, in seconds.
        float rate_clock;

        // Direction of spread
        glm::vec2 spread_direction;

        // Angle of spread in radiants
        float spread_angle;

        // Min/Max particle speed
        glm::vec2 speed;

        // Min/Max lifetime of particle
        glm::vec2 lifetime;

        // Min/Max scale of particle
        glm::vec2 scale;

        // Tint color
        engine::Color color;

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
                        engine::Color color)
            : rate(rate),
              rate_clock(0.0),
              spread_direction(spread_direction),
              spread_angle(spread_angle),
              speed(speed_min, speed_max),
              lifetime(lifetime_min, lifetime_max),
              scale(scale_min, scale_max),
              color(color)
        {
        }
    };
} // namespace tilegame::components
