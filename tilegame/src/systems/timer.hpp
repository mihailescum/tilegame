#pragma once

#include <vector>

#include "system.hpp"

namespace tilegame::systems
{
    class Timer : public System
    {

    public:
        Timer(tilegame::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
        void end_update();
    };
} // namespace tilegame