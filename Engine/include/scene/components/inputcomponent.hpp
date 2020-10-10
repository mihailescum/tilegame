#pragma once

#include <unordered_map>
#include <GLFW/glfw3.h>

namespace engine
{
    struct InputComponent : public std::unordered_map<int, std::pair<bool, bool>>
    {
        
    };
} // namespace engine