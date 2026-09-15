#pragma once

#include <string>

#include <glm/glm.hpp>

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief World-space position of an entity. Exposed to Lua as `_Transform`.
     */
    struct Transform
    {
        glm::vec2 position;

        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame
