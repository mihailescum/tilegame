#pragma once

#include "glm/glm.hpp"

namespace tilegame::components
{
    struct Transform
    {
        glm::vec2 position_local;
        glm::vec2 position_global;
    };
} // namespace tilegame
