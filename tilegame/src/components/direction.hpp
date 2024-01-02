#pragma once

#include <string>
#include <optional>

#include <glm/glm.hpp>

#include "sol/sol.hpp"

namespace tilegame::components
{
    struct Direction
    {
        glm::vec2 direction;

        Direction() : Direction(glm::vec2(0.0)) {}
        Direction(const glm::vec2 &direction) : direction(direction) {}

        const glm::vec2 &operator()() const { return direction; }
        glm::vec2 &operator()() { return direction; }
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
