#pragma once

#include <string>

#include "core/spritesheet.hpp"

namespace engine
{
    struct SpriteSheetComponent
    {
    public:
        SpriteSheetComponent() = default;
        SpriteSheetComponent(const SpriteSheetComponent &spriteSheetComponent) = default;
        SpriteSheetComponent(const SpriteSheet *spriteSheet, const int spriteId, const std::string &currentState, const int currentFrame);

        const SpriteSheet *spriteSheet;
        int spriteId;
        std::string currentState;
        int currentFrame;
    };
} // namespace engine