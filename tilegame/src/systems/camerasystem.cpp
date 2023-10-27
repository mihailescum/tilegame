#include "camerasystem.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "components/camera.hpp"

namespace tilegame::systems
{
    CameraSystem::CameraSystem(engine::Scene &scene, entt::registry &registry) : System(scene, registry)
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
