#include "core/spriteinfo.hpp"

namespace engine
{
    void SpriteInfo::addSpriteState(const std::string &name, const Rectangle frameSourceRect)
    {
        if (this->spriteStates.count(name))
            this->spriteStates[name].push_back(frameSourceRect);
        else
        {
            std::vector<Rectangle> newState;
            newState.push_back(frameSourceRect);
            this->spriteStates.emplace(name, newState);
        }
    }
} // namespace engine