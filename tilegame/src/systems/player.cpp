#include "player.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "components/player.hpp"
#include "components/movement.hpp"
#include "components/direction.hpp"
#include "components/transform.hpp"
#include "components/scenenode.hpp"
#include "components/renderable2d.hpp"
#include "components/ordering.hpp"
#include "components/sprite.hpp"
#include "components/animation.hpp"
#include "components/inactive.hpp"
#include "components/speed.hpp"
#include "components/collider.hpp"

namespace tilegame::systems
{
    Player::Player(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Player::initialize()
    {
    }

    void Player::load_content()
    {
        auto &resource_manager = _scene.game().resource_manager();
        const engine::tilemap::Tileset *characters = resource_manager.load_resource<engine::tilemap::Tileset>("characters", "content/characters/characters.tsj");
        const engine::Texture2D &characters_texture = characters->texture();
        const engine::Texture2D &characters_texture_luminosity = characters->luminosity_texture();

        const engine::graphics::Sprite &player1_sprite = (*characters)["man"];

        _player1_entity = _registry.create();
        _registry.emplace<components::Player>(_player1_entity, 1);
        _registry.emplace<components::Transform>(_player1_entity, glm::vec2(80, 80));
        _registry.emplace<components::Ordering>(_player1_entity, 2.0);
        _registry.emplace<components::Direction>(_player1_entity);
        _registry.emplace<components::Movement>(_player1_entity);
        _registry.emplace<components::Speed>(_player1_entity, 200.0);

        // const tilegame::SceneGraphData player1_scenedata(_player1_entity);
        // tilegame::SceneGraphNode &player1_scenenode = _scene.scene_graph_root().add_child(player1_scenedata);
        //_registry.emplace<components::SceneNode>(_player1_entity, &player1_scenenode);

        const auto &player1_animation_component = _registry.emplace<components::Animation>(_player1_entity, 0.0, 0, player1_sprite["down_walking"].frames);
        _registry.emplace<components::Renderable2D>(_player1_entity);
        _registry.emplace<components::Sprite>(_player1_entity, engine::Texture2DContainer<2>{&characters_texture, &characters_texture_luminosity}, player1_animation_component.get_current_frame().source_rect);

        const auto current_animation_tile = characters->get(player1_animation_component.get_current_frame().id);
        if (current_animation_tile->collision_shape)
        {
            _registry.emplace<components::Collider>(_player1_entity, std::unique_ptr<engine::Shape>(current_animation_tile->collision_shape->clone()));
        }
    }

    void Player::update(const engine::GameTime &update_time)
    {
        const auto players = _registry.view<const components::Player, const components::Speed>(entt::exclude<components::Inactive>);

        for (auto &&[entity, player, velocity] : players.each())
        {
            switch (player())
            {
            case 1:
            {
                glm::vec2 direction = handle_input_1();
                _registry.patch<components::Direction>(entity,
                                                       [direction](auto &comp)
                                                       {
                                                           comp.direction = direction;
                                                       });
            }
            break;

            default:
                throw "Unknown player ID";
                break;
            }
        }
    }

    glm::vec2 Player::handle_input_1()
    {
        const auto &window = _scene.game().window();

        auto result = glm::vec2(0.0);
        if (window.is_key_pressed(GLFW_KEY_LEFT))
        {
            result.x -= 1.0;
        }

        if (window.is_key_pressed(GLFW_KEY_RIGHT))
        {
            result.x += 1.0;
        }

        if (window.is_key_pressed(GLFW_KEY_UP))
        {
            result.y -= 1.0;
        }

        if (window.is_key_pressed(GLFW_KEY_DOWN))
        {
            result.y += 1.0;
        }

        // For some reason, when Up + Down are pressed, and later Left as well, GLFW does not recognize that Left is pressed.

        if (glm::length2(result) > 1e-10)
        {
            return glm::normalize(result);
        }
        else
        {
            return glm::vec2(0.0);
        }
    }
} // namespace tilegame::systems
