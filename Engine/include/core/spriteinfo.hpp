#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "core/rectangle.hpp"

namespace engine
{
    struct SpriteInfo
    {
    public:
        SpriteInfo(const int id) : id(id) {}

        const int id;
        std::unordered_map<std::string, std::vector<Rectangle>> spriteStates;

        void addSpriteState(const std::string &name, const Rectangle frameSourceRect);
    };
} // namespace engine