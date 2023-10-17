#include "scene/components/spriteinfocomponent.hpp"

namespace engine
{
    SpriteInfoComponent::SpriteInfoComponent(const SpriteInfo *spriteInfo, const std::string &currentState, const int currentFrame)
        : spriteInfo(spriteInfo), currentState(currentState), currentFrame(currentFrame)
    {
    }
} // namespace engine