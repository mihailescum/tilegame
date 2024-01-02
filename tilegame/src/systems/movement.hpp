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
        void check_target_reached();

    public:
        Movement(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
        void end_update();
    };
} // namespace tilegame