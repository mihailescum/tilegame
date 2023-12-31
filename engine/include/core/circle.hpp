#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "core/shape.hpp"

namespace engine
{
    struct Cirle : public Shape
    {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat radius;

        Cirle() : Cirle(0.0f, 0.0f, 0.0f) {}
        Cirle(GLfloat x, GLfloat y, GLfloat radius) : x(x), y(y), radius(radius) {}
    };
} // namespace engine
