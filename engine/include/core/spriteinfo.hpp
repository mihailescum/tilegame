#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "rectangle.hpp"

namespace engine
{
    struct SpriteInfo
    {
    public:
        SpriteInfo(std::size_t id) : id(id) {}

        std::size_t id;
        std::unordered_map<std::string, std::vector<Rectangle>> spriteStates;

        void addSpriteState(const std::string &name, const Rectangle frameSourceRect);
    };
} // namespace engine