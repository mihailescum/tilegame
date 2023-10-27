#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class MovementSystem : public System
    {
    private:
    public:
        MovementSystem(engine::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame