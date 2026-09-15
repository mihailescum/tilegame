#pragma once

#include <vector>

#include "entt/entt.hpp"

namespace tilegame::components
{
    /**
     * @brief Object pool of Particle entities owned by a ParticleEmitter, avoiding per-particle
     * entity creation at emission time. `container` is partitioned so that entities before
     * `first_dead_particle` are alive/active and the rest are dead/Inactive, ready to be recycled;
     * the Particle system grows it (by `growth_factor`) once occupancy passes `growth_threshold`.
     */
    struct ParticlePool
    {
        std::vector<entt::entity> container;
        /// Index of the first dead (inactive, reusable) slot in `container`; entities before it are alive.
        std::size_t first_dead_particle;
        /// Occupancy fraction (of `container.size()`) that triggers growing the pool.
        float growth_threshold;
        /// Multiplier applied to the pool's capacity when it grows.
        float growth_factor;

        ParticlePool()
            : container(), first_dead_particle(0), growth_threshold(0.8), growth_factor(1.5) {}
    };
} // namespace tilegame::components
