#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "core/shape.hpp"

namespace engine
{
    struct Circle : public Shape
    {
    public:
        glm::vec2 origin;
        GLfloat radius;

        Circle() : Circle(glm::vec2(0.0), 0.0f) {}
        Circle(const glm::vec2 &origin, GLfloat radius) : origin(origin), radius(radius) {}

        inline Shape *clone() const override { return new Circle(*this); }

        inline Circle operator+(const glm::vec2 &other) const
        {
            Circle result(origin + other, radius);
            return result;
        }

        inline Circle operator-(const glm::vec2 &other) const
        {
            Circle result = *this + (-other);
            return result;
        }
    };
} // namespace engine
