#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class MovementController : public System
    {
    private:
        void update_movement(const engine::GameTime &update_time);
        void update_direction_from_target(const engine::GameTime &update_time);
        void add_target_component(entt::registry &registry, entt::entity entity);

    public:
        MovementController(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
        void end_update();
    };
} // namespace tilegame