#pragma once

#include <string>

#include "sol/sol.hpp"

namespace tilegame::components
{
    struct LuaTable
    {
        sol::table table;

        LuaTable() = default;
        LuaTable(const sol::table &table) : table(table) {}

        const sol::table &operator()() const { return table; }
        sol::table &operator()() { return table; }
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
