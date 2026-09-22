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
     * EventListener<MapEnteredEvent>. Source is set to the entity that moved and target to
     * `map`, so Lua can scope a listener to one specific map regardless of which entity enters it
     * via `_add_event_listener(_MapEnteredEvent, callback, _null_entity, map_entity)` (see
     * content/scripts/map1.lua). A plain value type - never stored as an entt component - that
     * only exists for the duration of that call. Exposed to Lua as `_MapEnteredEvent`.
     */
    struct MapEnteredEvent
    {
        inline static const std::string EVENT_TYPE = "MAP_ENTERED_EVENT";

        MapEnteredEvent() {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };

    /**
     * @brief Raised immediately by the Movement system (via System::raise_event()) when an
     * entity's CurrentMap changes away from `map`, and delivered synchronously to every
     * EventListener<MapLeftEvent>. Source is set to the entity that moved and target to `map`,
     * so Lua can scope a listener to one specific map regardless of which entity leaves it via
     * `_add_event_listener(_MapLeftEvent, callback, _null_entity, map_entity)` (see
     * content/scripts/map1.lua). A plain value type - never stored as an entt component - that
     * only exists for the duration of that call. Exposed to Lua as `_MapLeftEvent`.
     */
    struct MapLeftEvent
    {
        inline static const std::string EVENT_TYPE = "MAP_LEFT_EVENT";

        MapLeftEvent() {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
