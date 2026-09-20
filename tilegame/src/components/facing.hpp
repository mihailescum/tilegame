#pragma once

#include <string>

#include <glm/glm.hpp>

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Last non-zero heading an entity moved in. Unlike Direction (which goes to zero at
     * rest so MovementController stops the entity), this is only overwritten when Direction is
     * non-zero (by systems::Facing), so it keeps pointing the way the entity was last walking
     * while it stands still. Used by systems::Interaction to tell which way a stationary player
     * is facing, and by systems::SpriteOrientation to pick a directional sprite's Animation
     * state. Exposed to Lua as `_Facing`.
     */
    struct Facing
    {
        glm::vec2 direction;

        Facing() : Facing(glm::vec2(0.0, 1.0)) {}
        Facing(const glm::vec2 &direction) : direction(direction) {}

        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
