#include "playersystem.hpp"

#include <glm/glm.hpp>

#include "components/player.hpp"
#include "components/movement.hpp"
#include "components/transform.hpp"
#include "components/scenenode.hpp"
#include "components/renderable2d.hpp"
#include "components/ordering.hpp"
#include "components/sprite.hpp"
#include "components/animation.hpp"

namespace tilegame::systems
{
    PlayerSystem::PlayerSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void PlayerSystem::initialize()
    {
        _player1_entity = _registry.create();
        _registry.emplace<tilegame::components::Player>(_player1_entity, 1);
        _registry.emplace<tilegame::components::Transform>(_player1_entity, glm::vec2(100, 100), glm::vec2(0.0));
        _registry.emplace<tilegame::components::Ordering>(_player1_entity, 2.0);
        _registry.emplace<tilegame::components::Movement>(_player1_entity, tilegame::components::Movement::None, 200.0);

        const tilegame::SceneGraphData player1_scenedata(_player1_entity);
        tilegame::SceneGraphNode &player1_scenenode = _scene.scene_graph_root().add_child(player1_scenedata);
        _registry.emplace<tilegame::components::SceneNode>(_player1_entity, &player1_scenenode);
    }

    void PlayerSystem::load_content()
    {
        auto &resource_manager = _scene.game().resource_manager();
        const engine::sprite::SpriteSheet *characters = resource_manager.load_resource<engine::sprite::SpriteSheet>("characters", "content/characters/characters.tsj");
        const engine::Texture2D &characters_texture = characters->texture();

        const engine::sprite::Sprite &player1_sprite = (*characters)["man"];

        const auto &player1_animation_component = _registry.emplace<tilegame::components::Animation>(_player1_entity, 0.0, 0, player1_sprite["down_walking"].frames);
        _registry.emplace<tilegame::components::Renderable2D>(_player1_entity);
        _registry.emplace<tilegame::components::Sprite>(_player1_entity, characters_texture, player1_animation_component.get_current_frame().source_rect);
    }

    void PlayerSystem::update(const engine::GameTime &update_time)
    {
        const auto players = _registry.view<tilegame::components::Player>();

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
        const auto &window = _scene.game().window();

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

        // For some reason, when Up + Down are pressed, and later Left as well, GLFW does not recognize that Left is pressed.

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
