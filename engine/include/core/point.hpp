#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "shape.hpp"

namespace engine
{
    struct Point : public Shape
    {
    public:
        Point() : Point(0.0f, 0.0f) {}
        Point(GLfloat x, GLfloat y) : x(x), y(y) {}

        GLfloat x;
        GLfloat y;

        static const Point ZERO;

        inline Point operator+(const glm::vec2 &other) const
        {
            Point result(x + other.x, y + other.y);
            return result;
        }

        inline Point operator-(const glm::vec2 &other) const
        {
            Point result = *this + (-other);
            return result;
        }

        operator glm::vec2() const
        {
            return glm::vec2(x, y);
        }
    };
} // namespace engine
