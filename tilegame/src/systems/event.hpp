#pragma once

#include <queue>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class Event : public System
    {
    public:
        Event(tilegame::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
