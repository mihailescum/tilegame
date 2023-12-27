#pragma once

#include <functional>

#include "engine.hpp"

namespace tilegame::components
{
    struct TileMap
    {
        std::reference_wrapper<const engine::tilemap::TileMap> map;
        const engine::tilemap::TileMap &operator()() const { return map.get(); }
    };
} // namespace tilegame
