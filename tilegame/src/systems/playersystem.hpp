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
        void handle_input_1(tilegame::components::Movement &movement);

    public:
        PlayerSystem(engine::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame