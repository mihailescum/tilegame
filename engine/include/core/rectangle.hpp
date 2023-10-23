#pragma once

#include "glad/glad.h"

namespace engine
{
    struct Rectangle
    {
    public:
        Rectangle() {}
        Rectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height) : x(x), y(y), width(width), height(height) {}

        GLfloat x;
        GLfloat y;
        GLfloat height;
        GLfloat width;
    };
} // namespace engine
