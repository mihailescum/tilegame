#pragma once

#include "glad/glad.h"

namespace engine
{
    struct Rectangle
    {
    public:
        Rectangle() {}
        Rectangle(GLfloat x, GLfloat y, GLuint width, GLuint height) : x(x), y(y), width(width), height(height) {}

        GLfloat x;
        GLfloat y;
        GLuint height;
        GLuint width;
    };
} // namespace engine
