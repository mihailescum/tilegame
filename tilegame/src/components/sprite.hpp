#pragma once

#include "engine.hpp"

namespace tilegame::components
{
    struct Sprite
    {
        // textures[0] is the actual texture
        // textures[1] is the luminosity texture
        engine::Texture2DContainer<2> textures;
        engine::Rectangle source_rect;
    };
} // namespace tilegame
