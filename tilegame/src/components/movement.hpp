#pragma once

#include <glm/glm.hpp>

namespace tilegame::components
{
    struct Movement
    {
        glm::vec2 direction;
        float speed;
    };
} // namespace tilegame
