#pragma once

#include <string>

#include <glm/glm.hpp>

#include "sol/sol.hpp"

namespace tilegame::components
{
    struct Transform
    {
        glm::vec2 position;

        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame
