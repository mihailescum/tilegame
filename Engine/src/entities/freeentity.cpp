#include "entities/freeentity.hpp"

#include <glm/glm.hpp>

namespace engine
{
    FreeEntity::FreeEntity() : Entity()
    {
    }

    void FreeEntity::update(const double deltaTime)
    {
        glm::vec2 direction(0.0);
        if (static_cast<int>(this->movingDirection & MovingDirection::Left))
            direction.x -= 1.0;
        if (static_cast<int>(this->movingDirection & MovingDirection::Right))
            direction.x += 1.0;
        if (static_cast<int>(this->movingDirection & MovingDirection::Up))
            direction.y -= 1.0;
        if (static_cast<int>(this->movingDirection & MovingDirection::Down))
            direction.y += 1.0;

        if (glm::any(glm::greaterThan(glm::abs(direction), glm::vec2(1e-10))))
            direction = glm::normalize(direction);

        this->position += direction * (float)(this->speed * deltaTime);
        this->movingDirection = static_cast<MovingDirection>(0);
    }

    void FreeEntity::moveLeft() { this->movingDirection |= MovingDirection::Left; }
    void FreeEntity::moveRight() { this->movingDirection |= MovingDirection::Right; }
    void FreeEntity::moveUp() { this->movingDirection |= MovingDirection::Up; }
    void FreeEntity::moveDown() { this->movingDirection |= MovingDirection::Down; }
} // namespace engine