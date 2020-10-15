#include "worldscene/movementsystem.hpp"

#include "engine.hpp"

#include "worldscene/worldscene.hpp"

namespace tilegame::worldscene
{
    MovementSystem::MovementSystem(WorldScene &scene) : registry(scene.getRegistry()) {}

    void MovementSystem::initialize() {}

    void MovementSystem::update(const double deltaTime)
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

            moveComponent.direction = engine::MoveComponent::MoveDirection::None;

            if (glm::any(glm::greaterThan(glm::abs(direction), glm::vec2(1e-10))))
            {
                direction = glm::normalize(direction);

                glm::vec2 position = positionComponent.position + direction * (float)(moveComponent.speed * deltaTime);
                setPosition(entity, position.x, position.y);
            }
        }
    }

    void MovementSystem::setPosition(const entt::entity &entity, const float x, const float y)
    {
        this->registry.patch<engine::PositionComponent>(entity,
                                                        [=](auto &pos) {
                                                            pos.position.x = x;
                                                            pos.position.y = y;
                                                        });
    }
} // namespace tilegame::worldscene