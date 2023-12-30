#include "luatable.hpp"

#include <entt/core/type_info.hpp>

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string LuaTable::to_string() const
    {
        std::stringstream ss;
        ss << "LuaTable";
        return ss.str();
    }

    void LuaTable::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<LuaTable>();

        lua.new_usertype<LuaTable>(
            "_LuaTable",
            "type_id", &entt::type_hash<LuaTable>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return LuaTable(); },
                [](const sol::table &table)
                { return LuaTable(table); }),
            "table", &components::LuaTable::table,
            sol::meta_function::to_string, &LuaTable::to_string);
    }
} // namespace tilegame::components
