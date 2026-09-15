#pragma once

#include <string>

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Tag component excluding an entity from update/render processing; most systems filter
     * their views with `entt::exclude<Inactive>`. Used e.g. to park dead particles in their pool
     * without destroying the entity. Exposed to Lua as `_Inactive`.
     */
    struct Inactive
    {
        /// Keeps entt's sparse set from moving other elements on erase, since Inactive is toggled frequently.
        static constexpr auto in_place_delete = true;

        Inactive() = default;
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
