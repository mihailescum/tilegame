#include "worldscene.hpp"

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "engine.hpp"

namespace tilegame
{
    engine::Map *map1;
    std::unique_ptr<engine::WorldRenderSystem> worldRenderSystem;

    WorldScene::WorldScene(Tilegame &game) : spriteBatch(*game.getSpriteBatch()), engine::Scene(game)
    {
    }

    void WorldScene::initialize()
    {
        this->cameraObserver = std::make_unique<engine::Observer>(this->registry);
        this->cameraObserver->connectOnUpdate<engine::PositionComponent, engine::CameraComponent>();

        engine::Entity cameraEntity = this->createEntity();
        cameraEntity.add<engine::ChildComponent>();
        cameraEntity.add<engine::PositionComponent>();
        engine::CameraComponent &cameraComponent = cameraEntity.add<engine::CameraComponent>();
        cameraComponent.viewport = &game.getGraphicsDevice()->getViewport();

        this->getRegistry().patch<engine::PositionComponent>(cameraEntity, [](auto &pos) { pos.position = glm::vec2(0.0); });
    }

    void WorldScene::loadContent()
    {
        map1 = game.getResourceManager()->loadResource<engine::Map>("map1", "content/world/map1.tmx", this);
        worldRenderSystem = std::make_unique<engine::WorldRenderSystem>(*this);
        worldRenderSystem->initialize();
    }

    void WorldScene::unloadContent()
    {
    }

    void WorldScene::processInput() {}

    void WorldScene::update(const double deltaTime) {
        this->updateCameras();
    }

    void WorldScene::updateCameras()
    {
        auto cameras = this->getRegistry().view<engine::PositionComponent, engine::CameraComponent>();
        for (auto entity : *this->cameraObserver)
        {
            auto camera = cameras.get<engine::CameraComponent>(entity);
            auto position = cameras.get<engine::PositionComponent>(entity);

            float scale = 1.0f;
            camera.transform = glm::scale(glm::mat4(1.0), glm::vec3(scale));

            glm::vec3 translate(
                floor(position.position.x * scale) / scale, 
                floor(position.position.y * scale) / scale, 
                0.0);
            camera.transform = glm::translate(camera.transform, translate);
        }
    }

    void WorldScene::draw()
    {
        auto cameras = this->getRegistry().view<engine::CameraComponent>();
        for (auto entity : cameras)
        {
            const auto camera = cameras.get(entity);

            if (camera.viewport)
            {
                worldRenderSystem->draw(spriteBatch, camera);
            }
        }
    }
} // namespace tilegame