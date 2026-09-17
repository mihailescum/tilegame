#include "map.hpp"

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    void Map::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Map>();

        lua.new_usertype<Map>(
            "_Map",
            "type_id", &entt::type_hash<Map>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return Map{}; }));
    }
} // namespace tilegame::components
