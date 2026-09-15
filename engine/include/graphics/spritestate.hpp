#pragma once

#include <string>
#include <vector>

#include "graphics/spriteframe.hpp"

namespace engine::graphics
{
    /**
     * @brief A named animation state of a Sprite (e.g. "walk_down"), consisting of an
     * ordered sequence of frames to be played back.
     */
    struct SpriteState
    {
        std::string name;
        std::vector<SpriteFrame> frames;

        SpriteState() : SpriteState("") {}
        SpriteState(std::string name) : name(name) {}
    };

} // namespace engine::graphics
