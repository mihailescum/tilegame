#pragma once

#include "sol/sol.hpp"

namespace tilegame::components
{
    struct LuaTable
    {
        sol::table table;

        LuaTable(){};
        LuaTable(const sol::table &table) : table(table) {}

        const sol::table &operator()() const { return table; }
        sol::table &operator()() { return table; }
    };
} // namespace tilegame::components
