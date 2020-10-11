#include "scene/components/spritesheetcomponent.hpp"

namespace engine
{
    SpriteSheetComponent::SpriteSheetComponent(const SpriteSheet *spriteSheet, const int spriteId, const std::string &currentState, const int currentFrame)
        : spriteSheet(spriteSheet), spriteId(spriteId), currentState(currentState), currentFrame(currentFrame)
    {
    }
} // namespace engine