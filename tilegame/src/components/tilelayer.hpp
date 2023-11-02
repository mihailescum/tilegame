#pragma once

#include <functional>

#include "engine.hpp"

namespace tilegame::components
{
    struct TileLayer
    {
        std::reference_wrapper<const engine::tilemap::TileLayer> layer;
    };
} // namespace tilegame
