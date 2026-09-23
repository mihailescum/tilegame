#pragma once

#include <vector>

#include "engine.hpp"

namespace tilegame::systems
{
    /**
     * @brief Counts down Timer components and raises TimerEvent on expiry.
     *
     * Operates on entities with a Timer component, decrementing time_left
     * each frame; once it reaches zero, immediately raises a TimerEvent (via
     * the inherited System::raise_event(), delivered synchronously to
     * native and Lua listeners), either resetting the timer if it repeats or
     * removing the Timer component otherwise.
     */
    class Timer : public engine::System
    {

    public:
        Timer(engine::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame