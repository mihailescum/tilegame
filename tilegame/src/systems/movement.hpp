#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class Movement : public System
    {
    private:
        void apply_movement(const engine::GameTime &update_time);
        void update_move_to_target(const engine::GameTime &update_time);
        void add_movement_component(entt::registry &registry, entt::entity entity);

    public:
        Movement(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame