#pragma once

#include <functional>

#include "engine.hpp"

namespace tilegame::components
{
    struct TileLayer
    {
        const std::reference_wrapper<const engine::tilemap::TileLayer> layer;
    };
} // namespace tilegame
