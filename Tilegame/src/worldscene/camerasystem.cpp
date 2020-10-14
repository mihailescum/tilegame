#include "worldscene/camerasystem.hpp"

#include <glm/gtx/transform.hpp>

#include "engine.hpp"

namespace tilegame::worldscene
{
    void CameraSystem::initialize()
    {
        this->cameraObserver = std::make_unique<engine::Observer>(this->registry);
        this->cameraObserver->connectOnUpdate<engine::PositionComponent, engine::CameraComponent>();
    }

    void CameraSystem::update(const double deltaTime)
    {
        auto cameras = this->registry.view<engine::PositionComponent, engine::CameraComponent>();
        for (auto entity : *this->cameraObserver)
        {
            engine::CameraComponent &camera = cameras.get<engine::CameraComponent>(entity);
            const engine::PositionComponent position = cameras.get<engine::PositionComponent>(entity);

            float scale = 1.0f;
            camera.transform = glm::scale(glm::mat4(1.0), glm::vec3(scale));
            glm::vec3 translate(
                floor(-(position.x() - camera.viewport->width / 2) * scale) / scale,
                floor(-(position.y() - camera.viewport->height / 2) * scale) / scale,
                0.0);
            camera.transform = glm::translate(camera.transform, translate);
        }
    }
} // namespace tilegame::worldscene