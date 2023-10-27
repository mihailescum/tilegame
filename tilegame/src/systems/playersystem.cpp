#include "playersystem.hpp"

#include <glm/glm.hpp>

#include "components/player.hpp"
#include "components/movement.hpp"
#include "components/worldtransform.hpp"

namespace tilegame::systems
{
    PlayerSystem::PlayerSystem(engine::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void PlayerSystem::initialize()
    {
        const auto player1_entity = _registry.create();
        _registry.emplace<tilegame::components::Player>(player1_entity, 1);
        _registry.emplace<tilegame::components::WorldTransform>(player1_entity, glm::vec3(100, 100, 2));
        _registry.emplace<tilegame::components::Movement>(player1_entity, tilegame::components::Movement::None, 10.0);
    }

    void PlayerSystem::update(const engine::GameTime &update_time)
    {
        auto players = _registry.view<tilegame::components::Player, tilegame::components::Movement>();

        for (auto &&[entity, player, movement] : players.each())
        {
            switch (player.id)
            {
            case 1:
                handle_input_1(movement);
                break;

            default:
                throw "Unknown player ID";
                break;
            }
        }
    }

    void PlayerSystem::handle_input_1(tilegame::components::Movement &movement)
    {
        const auto &window = _scene.get_game().get_window();

        auto result = static_cast<tilegame::components::Movement::MovementDirection>(0);
        if (window.is_key_pressed(GLFW_KEY_LEFT))
        {
            result |= tilegame::components::Movement::Left;
        }

        if (window.is_key_pressed(GLFW_KEY_RIGHT))
        {
            result |= tilegame::components::Movement::Right;
        }

        if (window.is_key_pressed(GLFW_KEY_UP))
        {
            result |= tilegame::components::Movement::Up;
        }

        if (window.is_key_pressed(GLFW_KEY_DOWN))
        {
            result |= tilegame::components::Movement::Down;
        }
        movement.direction = result;
    }
} // namespace tilegame::systems
