#pragma once

#include <queue>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class EventSystem : public System
    {
    public:
        EventSystem(tilegame::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
