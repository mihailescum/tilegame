#pragma once

#include <string>

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Tracks the name of the map an entity's Transform currently falls within
     * (per World::map_at), so the Player system can detect transitions across frames.
     * Internal bookkeeping only, not exposed to Lua.
     */
    struct CurrentMap
    {
        std::string map_name;

        CurrentMap() = default;
        CurrentMap(const std::string &map_name) : map_name(map_name) {}
    };

    /**
     * @brief Raised immediately by the Movement system (via System::raise_event()) when an
     * entity's CurrentMap changes to `map_name`, and delivered synchronously to every
     * EventListener<MapEnteredEvent>. A plain value type - never stored as an entt component -
     * that only exists for the duration of that call. Exposed to Lua as `_MapEnteredEvent`.
     */
    struct MapEnteredEvent
    {
        inline static const std::string EVENT_TYPE = "MAP_ENTERED_EVENT";
        std::string map_name;

        MapEnteredEvent() = default;
        MapEnteredEvent(const std::string &map_name) : map_name(map_name) {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };

    /**
     * @brief Raised immediately by the Movement system (via System::raise_event()) when an
     * entity's CurrentMap changes away from `map_name`, and delivered synchronously to every
     * EventListener<MapLeftEvent>. A plain value type - never stored as an entt component -
     * that only exists for the duration of that call. Exposed to Lua as `_MapLeftEvent`.
     */
    struct MapLeftEvent
    {
        inline static const std::string EVENT_TYPE = "MAP_LEFT_EVENT";
        std::string map_name;

        MapLeftEvent() = default;
        MapLeftEvent(const std::string &map_name) : map_name(map_name) {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
