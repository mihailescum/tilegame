#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class MovementSystem : public System
    {
    private:
        void apply_movement(const engine::GameTime &update_time);
        void update_move_to_target(const engine::GameTime &update_time);

    public:
        MovementSystem(tilegame::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame