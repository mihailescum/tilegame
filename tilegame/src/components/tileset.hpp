#pragma once

#include <functional>

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief References the loaded tileset asset (source texture and tile definitions) used to
     * render a map's tile layers.
     */
    struct Tileset
    {
        std::reference_wrapper<const engine::tilemap::Tileset> tileset;

        const engine::tilemap::Tileset &operator()() const { return tileset.get(); }
    };
} // namespace tilegame
