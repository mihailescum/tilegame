#pragma once

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Lets a player trigger Interactable entities (typically NPCs) with Enter.
     *
     * Each frame, on a fresh Enter key press (edge-detected the same way as
     * systems::MessageBox), checks every player entity (Player + Transform + Facing) against
     * every Interactable entity (Transform + Interactable): among the ones within
     * kInteractionDistance and roughly in front of the player - dot product of the player's
     * Facing against the direction to the candidate at or above kInteractionFacingCosThreshold,
     * both hardcoded in interaction.cpp for now - the closest one raises an InteractEvent
     * (source = the interactable entity, payload = the player entity) via the inherited
     * System::raise_event().
     *
     * Must run after systems::MessageBox each frame (see WorldScene::update()). Skips the search
     * whenever a message is (or, this same frame, just was) on screen - MessageBoxState lives in
     * the registry's ctx(), so this only needs to read shared state, not call into MessageBox
     * directly. The "just was" half matters because MessageBox does its own independent Enter
     * edge-detection: without it, the very Enter press that closes a message (popping its last
     * line to empty) would, later in the same frame, also read as a fresh press here and
     * immediately open a new interaction; requiring the closed state to have already held for
     * one full frame avoids that. It also means Enter must be released and pressed again after
     * an interaction opens a message before that same message can be advanced. Lua reacts to the
     * event via `_add_event_listener(_InteractEvent, callback, npc_entity)`.
     */
    class Interaction : public System
    {
    private:
        bool _enter_was_down;
        // Whether a message was still on screen as of the end of the previous frame; see the
        // class comment for why this - not just the current MessageBoxState - gates the search.
        bool _message_was_open_previous_frame;

    public:
        Interaction(tilegame::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame
