#pragma once

#include "glad/glad.h"

#include "core/rectangle.hpp"

namespace engine
{
    struct Viewport
    {
    public:
        Viewport() = default;
        Viewport(const Rectangle &rect) : Viewport(rect.x, rect.y, rect.width, rect.height) {}
        Viewport(int x, int y, unsigned int width, unsigned int height) : x(x), y(y), width(width), height(height) {}

        int x;
        int y;
        unsigned int width;
        unsigned int height;
    };
} // namespace engine