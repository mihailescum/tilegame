#pragma once

#include "glm/glm.hpp"

namespace tilegame::components
{
    struct Transform
    {
        glm::vec2 position_local;
        glm::vec2 position_global;

        const glm::vec2 &operator()() const { return position_local; }
        glm::vec2 &operator()() { return position_local; }
    };
} // namespace tilegame
