#pragma once

#include <vector>

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Drives frame-by-frame sprite animation. The Animation system advances `clock` by
     * elapsed time and steps `current_frame_idx` through `frames`, looping back to the start once
     * the last frame's duration is exceeded, then updates any attached Sprite's source rect to match.
     */
    struct Animation
    {
        /// Time accumulated since the current frame started, in seconds.
        float clock;
        std::size_t current_frame_idx;
        std::vector<engine::graphics::SpriteFrame> frames;

        inline const engine::graphics::SpriteFrame &get_current_frame() const
        {
            return frames[current_frame_idx];
        }
    };
} // namespace tilegame
