#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "core/shape.hpp"

namespace engine
{
    struct Circle : public Shape
    {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat radius;

        Circle() : Circle(0.0f, 0.0f, 0.0f) {}
        Circle(GLfloat x, GLfloat y, GLfloat radius) : x(x), y(y), radius(radius) {}

        inline Shape *clone() const override { return new Circle(*this); }
    };
} // namespace engine
