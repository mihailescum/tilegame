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
    };
} // namespace tilegame
