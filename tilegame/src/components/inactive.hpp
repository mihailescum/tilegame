#pragma once

#include <string>

#include "sol/sol.hpp"

namespace tilegame::components
{
    struct Inactive
    {
        static constexpr auto in_place_delete = true;

        Inactive() = default;
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
