#pragma once

#include <string>
#include <vector>

#include "sprite/spriteframe.hpp"

namespace engine::sprite
{
    struct SpriteState
    {
        std::string name;
        std::vector<SpriteFrame> frames;

        SpriteState() : SpriteState("") {}
        SpriteState(std::string name) : name(name) {}
    };

} // namespace engine::sprite
