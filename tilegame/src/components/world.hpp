#pragma once

#include <functional>

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief References the loaded world asset (the set of maps positioned relative to
     * one another that make up the currently loaded game world). Lives on a single
     * dedicated entity so any system can look up world-space queries such as
     * World::to_global() without depending on the Map system that created it.
     */
    struct World
    {
        std::reference_wrapper<const engine::tilemap::World> world;
        const engine::tilemap::World &operator()() const { return world.get(); }
    };
} // namespace tilegame::components
