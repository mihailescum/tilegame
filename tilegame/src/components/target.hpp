#pragma once

#include <glm/glm.hpp>

namespace tilegame::components
{
    struct Target
    {
        glm::vec2 target;

        Target() : Target(glm::vec2(0.0)) {}
        Target(const glm::vec2 &target) : target(target) {}

        const glm::vec2 &operator()() const { return target; }
        glm::vec2 &operator()() { return target; }
    };
} // namespace tilegame::components
