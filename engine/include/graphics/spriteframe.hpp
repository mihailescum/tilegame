#pragma once

#include "core/rectangle.hpp"

namespace engine::graphics
{
    /**
     * @brief One frame of a sprite animation: the tileset-local tile id it comes
     * from, its display duration and its source rectangle within the sprite sheet
     * texture.
     */
    struct SpriteFrame
    {
        int id;
        float duration; ///< Display duration in seconds.
        engine::Rectangle source_rect;

        SpriteFrame(int id, float duration, const engine::Rectangle &source_rect)
            : id(id), duration(duration), source_rect(source_rect)
        {
        }
    };

} // namespace engine::graphics
