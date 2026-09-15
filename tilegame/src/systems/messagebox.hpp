#pragma once

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Drives dialog messages requested via a components::MessageBox component.
     *
     * Each frame, picks up at most one pending components::MessageBox (word-wrapping its
     * `text` into `lines` the moment it's found), tags every Player entity Inactive for as
     * long as it's active, and pops a line off the front on each Enter key press until none
     * are left, at which point it untags the Player entities, raises a MessageClosedEvent and
     * destroys the request entity. systems::Render is the one that actually draws `lines`
     * while the component exists; this system only owns the input-blocking/advancing logic.
     */
    class MessageBox : public System
    {
    private:
        entt::entity _active_entity;
        bool _enter_was_down;

    public:
        MessageBox(tilegame::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
        void end_update();
    };
} // namespace tilegame
