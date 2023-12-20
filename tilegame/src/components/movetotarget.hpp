#pragma once

#include <glm/glm.hpp>

namespace tilegame::components
{
    struct MoveToTarget
    {
        glm::vec2 start;
        glm::vec2 end;

        MoveToTarget() : MoveToTarget(glm::vec2(0.0), glm::vec2(0.0)) {}
        MoveToTarget(glm::vec2 start, glm::vec2 end) : start(start), end(end) {}
    };
} // namespace tilegame::components
