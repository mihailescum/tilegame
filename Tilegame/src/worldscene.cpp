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

        engine::Entity playerEntity = this->createEntity();
        playerEntity.add<engine::InputComponent>(std::vector<int>{GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN});
        playerEntity.add<engine::MoveComponent>(engine::MoveComponent::MoveDirection::None, 128);
        playerEntity.add<engine::PositionComponent>();
        engine::CameraComponent &cameraComponent = playerEntity.add<engine::CameraComponent>();
        cameraComponent.viewport = &game.getGraphicsDevice()->getViewport();
        this->registry.patch<engine::PositionComponent>(playerEntity, [](auto &pos) { pos.position = glm::vec2(0.0); });
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

    void WorldScene::processInput()
    {
        auto inputs = this->registry.view<engine::InputComponent>();
        for (auto entity : inputs)
        {
            engine::InputComponent &input = inputs.get(entity);
            for (auto kvp : input.keys)
            {
                kvp.second.second = kvp.second.first;
                kvp.second.first = game.getWindow()->isKeyPressed(kvp.first);

                // If the entity has a camera attached to it, it is the player
                if (this->getRegistry().has<engine::CameraComponent, engine::MoveComponent>(entity))
                    handlePlayerInput(engine::Entity(entity, this), kvp.first, kvp.second);
            }
        }
    }

    void WorldScene::handlePlayerInput(const entt::entity &entity, const int key, const std::pair<bool, bool> keyPressed)
    {
        engine::MoveComponent &moveComponent = this->registry.get<engine::MoveComponent>(entity);
        if (key == GLFW_KEY_DOWN && keyPressed.first)
        {
            moveComponent.direction |= engine::MoveComponent::MoveDirection::Down;
        }
        else if (key == GLFW_KEY_UP && keyPressed.first)
        {
            moveComponent.direction |= engine::MoveComponent::MoveDirection::Up;
        }
        else if (key == GLFW_KEY_LEFT && keyPressed.first)
        {
            moveComponent.direction |= engine::MoveComponent::MoveDirection::Left;
        }
        else if (key == GLFW_KEY_RIGHT && keyPressed.first)
        {
            moveComponent.direction |= engine::MoveComponent::MoveDirection::Right;
        }
    }

    void WorldScene::update(const double deltaTime)
    {
        this->updateMovables(deltaTime);
        this->updateCameras();
    }

    void WorldScene::updateMovables(const double deltaTime)
    {
        auto moveables = this->registry.view<engine::MoveComponent, engine::PositionComponent>();
        for (auto entity : moveables)
        {
            engine::MoveComponent &moveComponent = moveables.get<engine::MoveComponent>(entity);
            const engine::PositionComponent &positionComponent = moveables.get<engine::PositionComponent>(entity);

            glm::vec2 direction(0.0);
            if (static_cast<int>(moveComponent.direction & engine::MoveComponent::MoveDirection::Left))
                direction.x -= 1.0;
            if (static_cast<int>(moveComponent.direction & engine::MoveComponent::MoveDirection::Right))
                direction.x += 1.0;
            if (static_cast<int>(moveComponent.direction & engine::MoveComponent::MoveDirection::Up))
                direction.y -= 1.0;
            if (static_cast<int>(moveComponent.direction & engine::MoveComponent::MoveDirection::Down))
                direction.y += 1.0;

            if (glm::any(glm::greaterThan(glm::abs(direction), glm::vec2(1e-10))))
                direction = glm::normalize(direction);

            moveComponent.direction = engine::MoveComponent::MoveDirection::None;

            glm::vec2 position = positionComponent.position + direction * (float)(moveComponent.speed * deltaTime);
            this->registry.patch<engine::PositionComponent>(entity, [=](auto &pos) { pos.position = position; });
        }
    }

    void WorldScene::updateCameras()
    {
        auto cameras = this->registry.view<engine::PositionComponent, engine::CameraComponent>();
        for (auto entity : *this->cameraObserver)
        {
            engine::CameraComponent &camera = cameras.get<engine::CameraComponent>(entity);
            const engine::PositionComponent position = cameras.get<engine::PositionComponent>(entity);

            float scale = 1.0f;
            camera.transform = glm::scale(glm::mat4(1.0), glm::vec3(scale));

            glm::vec3 translate(
                -floor(position.position.x * scale) / scale,
                -floor(position.position.y * scale) / scale,
                0.0);
            camera.transform = glm::translate(camera.transform, translate);
        }
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