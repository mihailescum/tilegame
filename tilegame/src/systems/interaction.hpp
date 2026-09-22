#pragma once

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Lets a player trigger Interactable entities (typically NPCs) with Enter.
     *
     * Each frame, on a fresh Enter key press, checks every player entity (Player + Transform +
     * Facing) against every Interactable entity (Transform + Interactable): among the ones
     * within kInteractionDistance and roughly in front of the player - dot product of the
     * player's Facing against the direction to the candidate at or above
     * kInteractionFacingCosThreshold, both hardcoded in interaction.cpp for now - the closest one
     * raises an InteractEvent (source = the interactable entity, target/payload = the player
     * entity) via the inherited System::raise_event().
     *
     * Skips the search whenever components::MessageBoxOpenState (ctx()) says a message is
     * currently on screen. scenes::UIScene - which actually owns the message and its own Enter
     * handling - lives on a different scene, pushed on top of this one, and only updates after
     * this system does each frame (see SceneManager::update()); combined with that ctx() flag
     * only ever flipping at the start of a frame (see SceneManager::flush_pending()), it can
     * never change mid-frame out from under this check, so a plain Enter edge-detection here -
     * same as everywhere else - is enough: the very Enter press that closes a message still
     * reads as "message open" here since UIScene hasn't processed it yet this frame, and by next
     * frame that key press is no longer a fresh one anyway. Lua reacts to the event via
     * `_add_event_listener(_InteractEvent, callback, npc_entity)`.
     */
    class Interaction : public System
    {
    private:
        bool _enter_was_down;

    public:
        Interaction(tilegame::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame
