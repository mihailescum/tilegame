#pragma once

#include "engine.hpp"

namespace tilegame::components
{
    struct Sprite
    {
        const engine::Texture2D *texture;
        engine::Rectangle source_rect;
    };
} // namespace tilegame
