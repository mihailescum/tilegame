#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "components/camera.hpp"
#include "components/player.hpp"
#include "components/scenenode.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    Camera::Camera(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Camera::initialize()
    {
    }

    void Camera::load_content()
    {
        const auto camera_entity = _registry.create();
        _registry.emplace<components::Camera>(
            camera_entity,
            1.0,
            glm::mat4(1.0),
            _scene.game().graphics_device().viewport());
        _registry.emplace<components::Transform>(camera_entity, glm::vec2(0.0, 0.0), glm::vec2(0.0));

        entt::entity player1_entity = entt::null;
        auto players = _registry.view<const components::Player>(entt::exclude<components::Inactive>);
        for (auto &&[entity, player] : players.each())
        {
            if (player.id == 1)
            {
                player1_entity = entity;
                break;
            }
        }

        if (player1_entity != entt::null)
        {
            auto player1_scenenode = _registry.get<components::SceneNode>(player1_entity).node;

            const tilegame::SceneGraphData camera_scenedata(camera_entity);
            tilegame::SceneGraphNode &camera_scenenode = player1_scenenode->add_child(camera_scenedata);
            _registry.emplace<components::SceneNode>(camera_entity, &camera_scenenode);
        }
    }

    void Camera::update(const engine::GameTime &update_time)
    {
        const auto cameras = _registry.view<components::Camera, const components::Transform>(entt::exclude<components::Inactive>);

        for (auto &&[entity, camera, transform] : cameras.each())
        {
            glm::vec2 position = transform.position_global;
            float scale = camera.scale;

            glm::vec3 translate(
                floor(-(position.x - camera.viewport.width / 2) * scale) / scale,
                floor(-(position.y - camera.viewport.height / 2) * scale) / scale,
                0.0);

            // TODO use patch
            camera.transform = glm::translate(glm::mat4(1.0), translate);
            camera.transform = glm::scale(camera.transform, glm::vec3(scale));
        }
    }
} // namespace tilegame::systems
