#include "sprites/sprite.hpp"

namespace engine
{
    const std::string Sprite::DEFAULT_STATE = "DEFAULT";

    Sprite::Sprite(const int id, const int defaultFrame, const Texture2D &texture, const int frameWidth, const int frameHeight, const int columnsSpriteSheet, const int rowsSpriteSheet)
        : id(id),
          texture(texture),
          frameWidth(frameWidth),
          frameHeight(frameHeight),
          columnsSpriteSheet(columnsSpriteSheet),
          rowsSpriteSheet(rowsSpriteSheet),
          currentState(DEFAULT_STATE),
          currentFrame(defaultFrame)
    {
        this->addSpriteState(this->currentState, defaultFrame);
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

    Rectangle Sprite::getCurrentSourceRect() const
    {
        Rectangle result(
            (this->currentFrame % columnsSpriteSheet) * this->frameWidth,
            (this->currentFrame / columnsSpriteSheet) * this->frameHeight,
            this->frameWidth,
            this->frameHeight);
        return result;
    }

    const Texture2D &Sprite::getTexture() const { return this->texture; }
} // namespace engine