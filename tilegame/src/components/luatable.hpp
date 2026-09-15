#pragma once

#include <string>

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Holds the Lua table returned by an entity's script, as loaded by the Script system when
     * it executes a ScriptLoader's file. Lets other Lua code retrieve script-defined state/behavior
     * for that entity. Exposed to Lua as `_LuaTable`.
     */
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
