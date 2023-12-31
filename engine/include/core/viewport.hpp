#pragma once

#include "glad/glad.h"

#include "rectangle.hpp"

namespace engine
{
    struct Viewport
    {
    public:
        Viewport() : Viewport(0,0,0,0) {}
        Viewport(const Rectangle &rect) : Viewport(rect.x, rect.y, rect.width, rect.height) {}
        Viewport(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}

        int x;
        int y;
        int width;
        int height;
    };
} // namespace engine