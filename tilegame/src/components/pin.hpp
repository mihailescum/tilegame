#pragma once

#include <string>

#include "entt/entt.hpp"

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Locks this entity's Transform position to that of `parent` each frame, updated by the
     * Pin system. Used e.g. to attach the Camera to the player entity. Exposed to Lua as `_Pin`.
     */
    struct Pin
    {
        entt::entity parent;

        Pin() : Pin(static_cast<entt::entity>(entt::null)) {}
        Pin(entt::entity parent) : parent(parent) {}

        const entt::entity &operator()() const { return parent; }
        entt::entity &operator()() { return parent; }
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame
