#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "shape.hpp"

namespace engine
{
    struct Rectangle : public Shape
    {
    public:
        Rectangle() {}
        Rectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height) : x(x), y(y), width(width), height(height) {}

        GLfloat x;
        GLfloat y;
        GLfloat width;
        GLfloat height;

        static const Rectangle EMPTY;

        inline Rectangle operator+(const glm::vec2 &other) const
        {
            Rectangle result(x + other.x, y + other.y, width, height);
            return result;
        }

        inline Rectangle operator-(const glm::vec2 &other) const
        {
            Rectangle result = *this + (-other);
            return result;
        }
    };
} // namespace engine
