#include "core/spriteinfo.hpp"

namespace engine
{
    void SpriteInfo::addSpriteState(const std::string &name, const Rectangle frameSourceRect)
    {
        if (this->sprite_states.count(name))
            this->sprite_states[name].push_back(frameSourceRect);
        else
        {
            std::vector<Rectangle> newState;
            newState.push_back(frameSourceRect);
            this->sprite_states.emplace(name, newState);
        }
    }
} // namespace engine