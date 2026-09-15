#pragma once

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Static image drawn for a Renderable2D entity: the texture pair plus the source rect
     * within it (updated frame-to-frame by the Animation system when one is present).
     */
    struct Sprite
    {
        // textures[0] is the actual texture
        // textures[1] is the luminosity texture
        engine::Texture2DContainer<2> textures;
        engine::Rectangle source_rect;
    };
} // namespace tilegame
