#pragma once

#include "sol/sol.hpp"

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Static image drawn for a Renderable2D entity: the texture pair plus the source rect
     * within it (updated frame-to-frame by the Animation system when one is present). Exposed to
     * Lua as `_Sprite`, built from a texture pair returned by `_load_texture` and a source rect.
     */
    struct Sprite
    {
        // textures[0] is the actual texture
        // textures[1] is the luminosity texture
        engine::Texture2DContainer<2> textures;
        engine::Rectangle source_rect;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame
