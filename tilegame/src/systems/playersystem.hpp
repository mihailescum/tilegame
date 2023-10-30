#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"
#include "components/movement.hpp"

namespace tilegame::systems
{
    class PlayerSystem : public System
    {
    private:
        tilegame::components::Movement::MovementDirection handle_input_1();

    public:
        PlayerSystem(engine::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame