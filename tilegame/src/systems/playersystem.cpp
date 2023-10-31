#include "playersystem.hpp"

#include <glm/glm.hpp>

#include "components/player.hpp"
#include "components/movement.hpp"
#include "components/transform.hpp"
#include "components/scenenode.hpp"

namespace tilegame::systems
{
    PlayerSystem::PlayerSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void PlayerSystem::initialize()
    {

        auto player1_entity = _registry.create();
        _registry.emplace<tilegame::components::Player>(player1_entity, 1);
        _registry.emplace<tilegame::components::Transform>(player1_entity, glm::vec2(100, 100), glm::vec2(0.0));
        _registry.emplace<tilegame::components::Movement>(player1_entity, tilegame::components::Movement::None, 100.0);

        tilegame::SceneGraphData player1_scenedata(player1_entity);
        auto player1_scenenode = _scene.get_scene_graph_root().add_child(player1_scenedata);
        _registry.emplace<tilegame::components::SceneNode>(player1_entity, player1_scenenode);
    }

    void PlayerSystem::update(const engine::GameTime &update_time)
    {
        auto players = _registry.view<tilegame::components::Player>();

        for (auto &&[entity, player] : players.each())
        {
            switch (player.id)
            {
            case 1:
            {
                auto result = handle_input_1();
                _registry.patch<tilegame::components::Movement>(entity, [=](auto &movement)
                                                                { movement.direction = result; });
            }
            break;

            default:
                throw "Unknown player ID";
                break;
            }
        }
    }

    tilegame::components::Movement::MovementDirection PlayerSystem::handle_input_1()
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

        if ((result & tilegame::components::Movement::Left) && (result & tilegame::components::Movement::Right))
        {
            result &= ~tilegame::components::Movement::Left;
            result &= ~tilegame::components::Movement::Right;
        }

        if ((result & tilegame::components::Movement::Up) && (result & tilegame::components::Movement::Down))
        {
            result &= ~tilegame::components::Movement::Up;
            result &= ~tilegame::components::Movement::Down;
        }

        return result;
    }
} // namespace tilegame::systems
