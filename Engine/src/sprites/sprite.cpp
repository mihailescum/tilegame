#include "sprites/sprite.hpp"

namespace engine
{
    Sprite::Sprite(const int id, const Texture2D &texture, const int frameWidth, const int frameHeight, const int columnsSpriteSheet, const int rowsSpriteSheet)
        : id(id), texture(texture), frameWidth(frameWidth), frameHeight(frameHeight), columnsSpriteSheet(columnsSpriteSheet), rowsSpriteSheet(rowsSpriteSheet)
    {
    }

    void Sprite::addSpriteState(const std::string &name, const int frameNumber)
    {
        if (this->spriteStates.count(name))
            this->spriteStates[name].push_back(frameNumber);
        else
        {
            std::vector<int> newState;
            newState.push_back(frameNumber);
            this->spriteStates.emplace(name, newState);
        }
    }
} // namespace engine