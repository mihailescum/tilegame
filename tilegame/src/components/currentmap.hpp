#pragma once

#include <string>

#include "entt/entt.hpp"

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Tracks the map entity (per systems::World::map_at) an entity's Transform currently
     * falls within, so the Movement system can detect transitions across frames. Internal
     * bookkeeping only, not exposed to Lua.
     */
    struct CurrentMap
    {
        entt::entity map;

        CurrentMap() : map(entt::null) {}
        CurrentMap(entt::entity map) : map(map) {}

        entt::entity operator()() const { return map; }
    };

    /**
     * @brief Raised immediately by the Movement system (via System::raise_event()) when an
     * entity's CurrentMap changes to `map`, and delivered synchronously to every
     * EventListener<MapEnteredEvent>. A plain value type - never stored as an entt component -
     * that only exists for the duration of that call. Exposed to Lua as `_MapEnteredEvent`.
     */
    struct MapEnteredEvent
    {
        inline static const std::string EVENT_TYPE = "MAP_ENTERED_EVENT";
        entt::entity map;

        MapEnteredEvent() : map(entt::null) {}
        MapEnteredEvent(entt::entity map) : map(map) {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };

    /**
     * @brief Raised immediately by the Movement system (via System::raise_event()) when an
     * entity's CurrentMap changes away from `map`, and delivered synchronously to every
     * EventListener<MapLeftEvent>. A plain value type - never stored as an entt component -
     * that only exists for the duration of that call. Exposed to Lua as `_MapLeftEvent`.
     */
    struct MapLeftEvent
    {
        inline static const std::string EVENT_TYPE = "MAP_LEFT_EVENT";
        entt::entity map;

        MapLeftEvent() : map(entt::null) {}
        MapLeftEvent(entt::entity map) : map(map) {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
