#pragma once

#include <functional>

#include "engine.hpp"

namespace tilegame::components
{
    struct Tileset
    {
        const std::reference_wrapper<const engine::tilemap::Tileset> tileset;
    };
} // namespace tilegame
