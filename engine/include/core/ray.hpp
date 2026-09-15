#pragma once

#include <glm/glm.hpp>

#include "core/shape.hpp"

namespace engine
{
    /**
     * @brief Geometric primitive representing a line cast from an origin along a direction vector.
     *
     * Implements the Shape interface; primarily used together with
     * Rectangle::intersects() for ray casting against axis-aligned boxes.
     */
    struct Ray : public Shape
    {
    public:
        Ray() : Ray(glm::vec2(0.0), glm::vec2(0.0)) {}
        Ray(const glm::vec2 &origin, const glm::vec2 &direction) : origin(origin), direction(direction) {}

        inline Shape *clone() const override { return new Ray(*this); }

        glm::vec2 origin;
        glm::vec2 direction;
    };
} // namespace engine
