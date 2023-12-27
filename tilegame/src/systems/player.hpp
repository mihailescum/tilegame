#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"
#include "components/movement.hpp"

namespace tilegame::systems
{
    class Player : public System
    {
    private:
        entt::entity _player1_entity;
        glm::vec2 handle_input_1();

    public:
        Player(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame