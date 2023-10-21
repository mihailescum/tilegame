#pragma once

#include "glad/glad.h"

namespace engine
{
    struct Rectangle
    {
    public:
        Rectangle() {}
        Rectangle(GLfloat x, GLfloat y, GLint width, GLint height) : x(x), y(y), width(width), height(height) {}

        GLfloat x;
        GLfloat y;
        GLint height;
        GLint width;
    };
} // namespace engine
