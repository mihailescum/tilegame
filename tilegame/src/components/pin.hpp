#pragma once

#include <string>

#include <entt/entity/registry.hpp>

#include "sol/sol.hpp"

namespace tilegame::components
{
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
