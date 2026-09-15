#pragma once

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
} // namespace tilegame
