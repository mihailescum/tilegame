#pragma once

namespace engine
{
    struct Rectangle
    {
    public:
        Rectangle() {}
        Rectangle(const double x, const double y, const double width, const double height) : x(x), y(y), width(width), height(height) {}
        
        double x;
        double y;
        double height;
        double width;
    };
} // namespace engine
