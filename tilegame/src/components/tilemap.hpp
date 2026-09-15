#pragma once

#include <functional>

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief References the loaded tilemap asset a map entity is built from (dimensions, tile size,
     * associated layers/tilesets).
     */
    struct TileMap
    {
        std::reference_wrapper<const engine::tilemap::TileMap> map;
        const engine::tilemap::TileMap &operator()() const { return map.get(); }
    };
} // namespace tilegame
