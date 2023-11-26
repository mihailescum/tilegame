#pragma once

#include "engine.hpp"

namespace tilegame::components
{
    struct Particle
    {
        float lifetime_left;
        float scale;
        engine::Color color;
    };
} // namespace tilegame::components
