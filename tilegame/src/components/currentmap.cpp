#include "currentmap.hpp"

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string MapEnteredEvent::to_string() const
    {
        std::stringstream ss;
        ss << "MapEnteredEvent";
        return ss.str();
    }

    void MapEnteredEvent::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<MapEnteredEvent>();

        lua.new_usertype<MapEnteredEvent>(
            "_MapEnteredEvent",
            "type_id", &entt::type_hash<MapEnteredEvent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return MapEnteredEvent(); },
                [](entt::entity map)
                { return MapEnteredEvent(map); }),
            "map", &MapEnteredEvent::map,
            "EVENT_TYPE", sol::var(MapEnteredEvent::EVENT_TYPE.c_str()),
            sol::meta_function::to_string, &MapEnteredEvent::to_string);
    }

    std::string MapLeftEvent::to_string() const
    {
        std::stringstream ss;
        ss << "MapLeft";
        return ss.str();
    }

    void MapLeftEvent::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<MapLeftEvent>();

        lua.new_usertype<MapLeftEvent>(
            "_MapLeftEvent",
            "type_id", &entt::type_hash<MapLeftEvent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return MapLeftEvent(); },
                [](entt::entity map)
                { return MapLeftEvent(map); }),
            "map", &MapLeftEvent::map,
            "EVENT_TYPE", sol::var(MapLeftEvent::EVENT_TYPE.c_str()),
            sol::meta_function::to_string, &MapLeftEvent::to_string);
    }
} // namespace tilegame::components
