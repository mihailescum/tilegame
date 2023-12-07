#pragma once

#include <vector>

#include "engine.hpp"

namespace tilegame::components
{
    struct Animation
    {
        float clock;
        std::size_t current_frame_idx;
        std::vector<engine::graphics::SpriteFrame> frames;

        inline const engine::graphics::SpriteFrame &get_current_frame() const
        {
            return frames[current_frame_idx];
        }
    };
} // namespace tilegame
