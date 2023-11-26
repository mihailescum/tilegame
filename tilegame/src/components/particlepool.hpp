#pragma once

#include <vector>

#include <entt/entity/registry.hpp>

namespace tilegame::components
{
    struct ParticlePool
    {
        std::vector<entt::entity> container;
        std::size_t first_dead_particle;
        float growth_threshold;
        float growth_factor;

        ParticlePool()
            : container(), first_dead_particle(0), growth_threshold(0.8), growth_factor(1.5) {}
    };
} // namespace tilegame::components
