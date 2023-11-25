#pragma once

#include <glm/glm.hpp>

#include "engine.hpp"

namespace tilegame::components
{
    struct ParticleEmitter
    {
        // Particles per second
        int rate;

        // Direction of spread
        glm::vec2 spread_direction;

        // Angle of spread in radiants
        float spread_angle;

        // Min/Max lifetime of particle
        glm::vec2 lifetime;

        // Tint color
        engine::Color color;

        // Min/Max scale of particle
        glm::vec2 scale;
    };
} // namespace tilegame::components
