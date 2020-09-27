#pragma once

#include "glad/glad.h"

#include "core/rectangle.hpp"

namespace engine
{
    struct Viewport
    {
    public:
        Viewport(const int x, const int y, const int width, const int height) : x(x), y(y), width(width), height(height) {}
        Viewport(const Rectangle &rect) : Viewport(rect.x, rect.y, rect.width, rect.height) {}
        
        int x;
        int y;
        int width;
        int height;
    };
} // namespace engine