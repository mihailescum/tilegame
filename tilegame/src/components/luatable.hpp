#pragma once

#include "sol/sol.hpp"

namespace tilegame::components
{
    struct LuaTable
    {
        sol::table table;

        LuaTable(){};
        LuaTable(const sol::table &table) : table(table) {}
    };
} // namespace tilegame::components
