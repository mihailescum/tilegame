#include "worldscene.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace tilegame
{
    void WorldScene::initialize()
    {
        cameraObserver = std::make_unique<engine::Observer>(registry);
        cameraObserver->connectOnUpdate<engine::PositionComponent, engine::CameraComponent>();
    }

    void WorldScene::loadContent()
    {
    }

    void WorldScene::processInput()
    {
        auto inputRecievers = registry.view<engine::InputComponent>();
        for (auto entity : inputRecievers) {
            auto inputComponent = inputRecievers.get(entity);
            for (auto &kv : inputComponent) {
                kv.second.second = kv.second.first;
                kv.second.first = window;
            }
        }
    }


    void WorldScene::update(const double deltaTime)
    {
        updateCameras();
    }

    void WorldScene::updateCameras()
    {
        auto cameras = registry.view<engine::CameraComponent, engine::PositionComponent>();
        for (auto entity : *cameraObserver)
        {
            auto &camera = cameras.get<engine::CameraComponent>(entity);
            if (camera.viewport)
            {
                auto &pos = cameras.get<engine::PositionComponent>(entity);

                float scale = 1.0;
                glm::mat4 transform = glm::scale(glm::mat4(1.0), glm::vec3(scale));

                // translation - Inverting coorinates because translation matrix has to 'move' the objects into the different direction
                // translation is clipped to integers
                glm::vec3 translation(
                    floor(-(pos.position.x - camera.viewport->width / 2) * scale) / scale,
                    floor(-(pos.position.y - camera.viewport->height / 2) * scale) / scale,
                    0.0);
                transform = glm::translate(transform, translation);
                camera.transform = transform;
            }
        }
        cameraObserver->clear();
    }

    void WorldScene::draw()
    {
    }
} // namespace tilegame