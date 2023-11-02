#pragma once

#include <functional>

#include "engine.hpp"

namespace tilegame::components
{
    struct Tileset
    {
        std::reference_wrapper<const engine::tilemap::Tileset> tileset;
    };
} // namespace tilegame
