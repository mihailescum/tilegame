#pragma once

#include <vector>

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Counts down Timer components and raises TimerEvent on expiry.
     *
     * Operates on entities with a Timer component, decrementing time_left
     * each frame; once it reaches zero, emits a TimerEvent (raised to Lua
     * event listeners), either resetting the timer if it repeats or removing
     * the Timer component otherwise.
     */
    class Timer : public System
    {

    public:
        Timer(tilegame::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
        void end_update();
    };
} // namespace tilegame