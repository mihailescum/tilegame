#pragma once

#include "core/rectangle.hpp"

namespace engine::sprite
{
    struct SpriteFrame
    {
        unsigned int id;
        float duration;
        engine::Rectangle source_rect;

        SpriteFrame(unsigned int id, float duration, const engine::Rectangle &source_rect)
            : id(id), duration(duration), source_rect(source_rect)
        {
        }
    };

} // namespace engine::sprite
