#ifndef __ENGINE_SPRITE_H__
#define __ENGINE_SPRITE_H__

#include <unordered_map>
#include <vector>
#include <string>

#include "core/texture2D.hpp"

namespace engine
{
    class Sprite
    {
    private:
        const int id;
        const Texture2D &texture;
        const int frameWidth;
        const int frameHeight;
        const int rowsSpriteSheet;
        const int columnsSpriteSheet;

        std::unordered_map<std::string, std::vector<int>> spriteStates;

    public:
        Sprite(const int id, const Texture2D &texture, const int frameWidth, const int frameHeight, const int columnsSpriteSheet, const int rowsSpriteSheet);
        void addSpriteState(const std::string &name, const int frameNumber);
    };
} // namespace engine

#endif