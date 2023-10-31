#pragma once

#include <functional>

#include "engine.hpp"

namespace tilegame::components
{
    struct TileMap
    {
        const std::reference_wrapper<const engine::tilemap::TileMap> map;
    };
} // namespace tilegame
