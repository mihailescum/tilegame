#include "worldscene.hpp"

#include <memory>

#include "engine.hpp"

namespace tilegame
{
    std::unique_ptr<engine::Observer> cameraObserver;

    engine::Map *map1;
    std::unique_ptr<engine::Renderer> map1Renderer;

    WorldScene::WorldScene(engine::Game &game) : engine::Scene(game)
    {
    }

    void WorldScene::initialize()
    {
        this->spriteBatch = std::make_unique<engine::SpriteBatch>(*game.getGraphicsDevice());

        engine::Entity cameraEntity = this->createEntity();
        cameraEntity.add<engine::ChildComponent>();
        cameraEntity.add<engine::PositionComponent>();
        engine::CameraComponent &cameraComponent = cameraEntity.add<engine::CameraComponent>();
        cameraComponent.viewport = &game.getGraphicsDevice()->getViewport();

        this->getRegistry().patch<engine::PositionComponent>(cameraEntity, [](auto &pos) { pos.position = glm::vec2(0.0); });
    }

    void WorldScene::loadContent()
    {
        this->spriteBatch->create();

        //map1 = this->resourceManager->loadResource<engine::Map>("map1", "content/world/map1.tmx", "content/world/");
        //map1Renderer = std::make_unique<engine::Renderer>(*map1);
        //map1Renderer->initialize();
    }

    void WorldScene::unloadContent()
    {
        this->spriteBatch.reset();
    }

    void WorldScene::processInput() {}

    void WorldScene::update(const double deltaTime) {}

    void WorldScene::draw()
    {
        auto cameras = this->getRegistry().view<engine::CameraComponent>();
        for (auto entity : cameras)
        {
            const auto camera = cameras.get(entity);

            if (camera.viewport)
            {
                //map1Renderer->draw(*spriteBatch, camera);
            }
        }
    }
} // namespace tilegame