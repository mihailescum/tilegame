#pragma once

#include <string>
#include <vector>

#include "graphics/spriteframe.hpp"

namespace engine::graphics
{
    struct SpriteState
    {
        std::string name;
        std::vector<SpriteFrame> frames;

        SpriteState() : SpriteState("") {}
        SpriteState(std::string name) : name(name) {}
    };

} // namespace engine::graphics
