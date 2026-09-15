#pragma once

#include <string>
#include <optional>

#include <glm/glm.hpp>

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Normalized facing/movement direction of an entity, combined with Speed by the
     * MovementController system to compute per-frame Movement velocity. Exposed to Lua as `_Direction`.
     */
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
