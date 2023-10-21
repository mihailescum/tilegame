#pragma once

#include <functional>

#include "engine.hpp"

namespace tilegame::components
{
    struct Renderable2D
    {
    public:
        std::reference_wrapper<engine::Texture2D> texture;
    };
} // namespace tilegame
