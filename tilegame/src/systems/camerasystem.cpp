#include "camerasystem.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "components/camera.hpp"
#include "components/player.hpp"

namespace tilegame::systems
{
    CameraSystem::CameraSystem(engine::Scene &scene, entt::registry &registry, tilegame::SceneGraphNode &scene_graph_root) : System(scene, registry, scene_graph_root)
    {
    }

    void CameraSystem::initialize()
    {
        const auto camera_entity = _registry.create();
        _registry.emplace<tilegame::components::Camera>(
            camera_entity,
            glm::vec2(100.0, 100.0),
            1.0,
            glm::mat4(1.0),
            _scene.get_game().get_graphics_device().get_viewport());

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
        //_registry.emplace<tilegame::components::Parent>(camera_entity, player1_entity);
    }

    void CameraSystem::update(const engine::GameTime &update_time)
    {
        auto cameras = _registry.view<tilegame::components::Camera>();

        for (auto &&[entity, camera] : cameras.each())
        {
            glm::vec2 position = camera.look_at;
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
