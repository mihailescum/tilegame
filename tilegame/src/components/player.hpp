#pragma once

#include <string>

namespace tilegame::components
{
    /**
     * @brief Marks an entity as a player character and distinguishes it by `id` (e.g. for local
     * multiplayer). The Player system uses `id` to dispatch to the right input handler.
     */
    struct Player
    {
        const int id;
        int operator()() const { return id; }
    };

    /**
     * @brief Raised via System::raise() by the `_stop_player_input` Lua binding, immediately
     * delivered to systems::Player's EventListener<StopPlayerInputEvent> (registered on
     * `_player1_entity` in Player::load_content()), suppressing keyboard input for whichever
     * player entity's `Player::id` matches `player_id`. Not exposed to Lua as a usertype -
     * nothing outside systems::Player subscribes to it today - but carries EVENT_TYPE like any
     * other event since System::raise_event() needs it regardless.
     */
    struct StopPlayerInputEvent
    {
        inline static const std::string EVENT_TYPE = "STOP_PLAYER_INPUT_EVENT";

        int player_id;
    };

    /**
     * @brief Raised via System::raise() by the `_resume_player_input` Lua binding, immediately
     * delivered to systems::Player's EventListener<ResumePlayerInputEvent>, re-enabling
     * keyboard input for whichever player entity's `Player::id` matches `player_id`.
     */
    struct ResumePlayerInputEvent
    {
        inline static const std::string EVENT_TYPE = "RESUME_PLAYER_INPUT_EVENT";

        int player_id;
    };
} // namespace tilegame
