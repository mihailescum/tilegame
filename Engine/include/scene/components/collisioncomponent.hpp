#pragma once

#include <glm/glm.hpp>

namespace engine
{
    struct CollisionComponent
    {
    public:
        CollisionComponent() = default;
        CollisionComponent(const CollisionComponent &collisionComponent) = default;
        CollisionComponent(const glm::dvec2 &movementVector)
            : movementVector(movementVector) {}

        glm::dvec2 movementVector;
    };
} // namespace engine