#pragma once

#include <glm/glm.hpp>

#include "core/shape.hpp"

namespace engine
{
    struct Ray : public Shape
    {
    public:
        Rectangle() {}
        Rectangle(const glm::vec2 &origin, const glm::vec2 &direction) : origin(origin), direction(direction) {}

        glm::vec2 origin;
        glm::vec2 direction;
    };
} // namespace engine
