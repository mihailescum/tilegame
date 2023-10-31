#include "camerasystem.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "components/camera.hpp"
#include "components/player.hpp"
#include "components/scenenode.hpp"

namespace tilegame::systems
{
    CameraSystem::CameraSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void CameraSystem::initialize()
    {
        const auto camera_entity = _registry.create();
        _registry.emplace<tilegame::components::Camera>(
            camera_entity,
            1.0,
            glm::mat4(1.0),
            _scene.get_game().get_graphics_device().get_viewport());
        _registry.emplace<tilegame::components::Transform>(camera_entity, glm::vec2(100.0, 100.0), glm::vec2(0.0));

        entt::entity player1_entity = entt::null;
        auto players = _registry.view<tilegame::components::Player>();
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
            auto player1_scenenode = _registry.get<tilegame::components::SceneNode>(player1_entity).node;

            tilegame::SceneGraphData camera_scenedata(camera_entity);
            auto camera_scenenode = player1_scenenode->add_child(camera_scenedata);
            _registry.emplace<tilegame::components::SceneNode>(camera_entity, camera_scenenode);
        }
    }

    void CameraSystem::update(const engine::GameTime &update_time)
    {
        auto cameras = _registry.view<tilegame::components::Camera, tilegame::components::Transform>();

        for (auto &&[entity, camera, transform] : cameras.each())
        {
            glm::vec2 position = transform.position_global;
            float scale = camera.scale;

            glm::vec3 translate(
                floor(-(position.x - camera.viewport.width / 2) * scale) / scale,
                floor(-(position.y - camera.viewport.height / 2) * scale) / scale,
                0.0);
            camera.transform = glm::translate(glm::mat4(1.0), translate);
            camera.transform = glm::scale(camera.transform, glm::vec3(scale));
        }
    }
} // namespace tilegame::systems
