#ifndef __ENGINE_RECTANGLE_H__
#define __ENGINE_RECTANGLE_H__

namespace engine
{
    struct Rectangle
    {
    public:
        Rectangle() {}
        Rectangle(const int x, const int y, const int width, const int height) : x(x), y(y), width(width), height(height) {}
        
        int x;
        int y;
        int height;
        int width;
    };
} // namespace engine

#endif