#pragma once

#include <glm/glm.hpp>

#include "core/shape.hpp"

namespace engine
{
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
