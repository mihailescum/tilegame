#pragma once

#include <vector>

#include "engine.hpp"

namespace tilegame::components
{
    struct Animation
    {
        double clock;
        std::size_t current_frame_idx;
        std::vector<engine::sprite::SpriteFrame> frames;

        inline const engine::sprite::SpriteFrame &get_current_frame() const
        {
            return frames[current_frame_idx];
        }
    };
} // namespace tilegame
