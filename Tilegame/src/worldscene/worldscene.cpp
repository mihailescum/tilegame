#include "worldscene/worldscene.hpp"

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "engine.hpp"

#include "tilegame.hpp"

namespace tilegame::worldscene
{
    engine::Entity playerEntity;

    WorldScene::WorldScene(Tilegame &game) : spriteBatch(*game.getSpriteBatch()), engine::Scene(game)
    {
    }

    void WorldScene::initialize()
    {
        this->createSystems();

        playerEntity = this->createEntity();
        playerEntity.add<engine::InputComponent>(std::vector<int>{GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN});
        playerEntity.add<engine::MoveComponent>(engine::MoveComponent::MoveDirection::None, 128.0);
        playerEntity.add<engine::PositionComponent>();
        engine::CameraComponent &cameraComponent = playerEntity.add<engine::CameraComponent>();
        cameraComponent.viewport = &game.getGraphicsDevice()->getViewport();
        this->registry.patch<engine::PositionComponent>(playerEntity, [](auto &pos) { pos.position = glm::vec2(0.0); });
    }

    void WorldScene::createSystems()
    {
        this->contentSystem = std::make_unique<ContentSystem>(*this, *game.getResourceManager());
        this->contentSystem->initialize();

        this->inputSystem = std::make_unique<InputSystem>(*this, *game.getWindow());
        this->inputSystem->initialize();

        this->cameraSystem = std::make_unique<CameraSystem>(*this);
        this->cameraSystem->initialize();

        this->movementSystem = std::make_unique<MovementSystem>(*this);
        this->movementSystem->initialize();

        this->worldRenderSystem = std::make_unique<engine::WorldRenderSystem>(*this);
        this->worldRenderSystem->initialize();
    }

    void WorldScene::loadContent()
    {
        contentSystem->loadContent();

        engine::SpriteSheet *sheet = game.getResourceManager()->loadResource<engine::SpriteSheet>("sheet1", "content/sprites/sprite_sheet.tsx");
        engine::SpriteComponent &spriteComp = playerEntity.add<engine::SpriteComponent>(sheet->getTexture(), engine::Rectangle());
        engine::SpriteSheetComponent &spriteSheetComp = playerEntity.add<engine::SpriteSheetComponent>(sheet, 0, "LEFT", 0);
        playerEntity.add<engine::RenderComponent>(sheet->getFrameWidth(), sheet->getFrameHeight(), 1.0);

        spriteComp.sourceRectangle = spriteSheetComp.spriteSheet->getSpriteInfo(spriteSheetComp.spriteId).spriteStates.at(spriteSheetComp.currentState)[spriteSheetComp.currentFrame];
    }

    void WorldScene::unloadContent()
    {
        contentSystem->unloadContent();
    }

    void WorldScene::processInput()
    {
        inputSystem->processInput();
    }

    void WorldScene::update(const double deltaTime)
    {
        movementSystem->update(deltaTime);
        cameraSystem->update(deltaTime);
    }

    void WorldScene::draw()
    {
        auto cameras = this->registry.view<engine::CameraComponent>();
        for (auto entity : cameras)
        {
            const engine::CameraComponent &camera = cameras.get(entity);

            if (camera.viewport)
            {
                worldRenderSystem->draw(spriteBatch, camera);
            }
        }
    }
} // namespace tilegame