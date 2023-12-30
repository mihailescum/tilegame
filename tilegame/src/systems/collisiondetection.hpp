#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class CollisionDetection : public System
    {
    public:
        CollisionDetection(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame