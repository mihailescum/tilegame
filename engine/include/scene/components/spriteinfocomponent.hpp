#pragma once

#include <string>

#include "core/spritesheet.hpp"

namespace engine
{
    struct SpriteInfoComponent
    {
    public:
        SpriteInfoComponent() = default;
        SpriteInfoComponent(const SpriteInfoComponent &spriteSheetComponent) = default;
        SpriteInfoComponent(const SpriteInfo *spriteInfo, const std::string &currentState, const int currentFrame);

        const SpriteInfo *spriteInfo;
        std::string currentState;
        int currentFrame;
    };
} // namespace engine