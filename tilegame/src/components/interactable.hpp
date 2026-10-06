#pragma once

#include <string>

#include "entt/entt.hpp"

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Tag component marking an entity (typically an NPC) as interactable. The Interaction
     * system checks every such entity against the player's Transform/Facing each frame and
     * raises an InteractEvent when the player is close enough, facing it, and presses Enter.
     * Exposed to Lua as `_Interactable`.
     */
    struct Interactable
    {
        Interactable() = default;
        Interactable(float max_distance, float cos_max_angle) : max_distance(max_distance), cos_max_angle(cos_max_angle) {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);

        float max_distance;
        float cos_max_angle;
    };

    /**
     * @brief Raised immediately by the Interaction system (via System::raise_event()) when a
     * player, facing and near enough to an Interactable entity, presses Enter. Source is set to
     * the Interactable entity and target to the player entity (also `player` below), so Lua can
     * scope a listener to one specific NPC via `_add_event_listener(_InteractEvent, callback,
     * npc_entity)`, one specific player via `_add_event_listener(_InteractEvent, callback,
     * _null_entity, player_entity)`, or both. A plain value type - never stored as an entt
     * component - that only exists for the duration of that call. Exposed to Lua as
     * `_InteractEvent`.
     */
    struct InteractEvent
    {
        inline static const std::string EVENT_TYPE = "INTERACT_EVENT";

        InteractEvent() = default;
        InteractEvent(float distance, float cos_angle) : distance(distance), cos_angle(cos_angle) {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);

        float distance;
        float cos_angle;
    };
} // namespace tilegame::components
