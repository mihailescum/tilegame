#pragma once

#include <string>

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Generic holder for a Lua table attached to an entity, for other Lua code to read
     * back later. Exposed to Lua as `_LuaTable`.
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
