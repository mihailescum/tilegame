#pragma once

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Per-particle state for an entity pooled by a ParticlePool. The Particle system counts
     * `lifetime_left` down each frame and returns the entity to the pool (marking it Inactive) once
     * it reaches zero.
     */
    struct Particle
    {
        float lifetime_left;
        float scale;
        engine::Color color;
    };
} // namespace tilegame::components
