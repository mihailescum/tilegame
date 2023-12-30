#include "core/rectangle.hpp"
namespace engine
{
    const Rectangle Rectangle::EMPTY(0.0f, 0.0f, 0.0f, 0.0f);

    bool Rectangle::intersects(const Rectangle &other)
    {
        return (x <= other.x + other.width && x + width >= other.x) &&
               (y <= other.y + other.height && y + height >= other.y);
    }

    bool Rectangle::intersects(const Point &other)
    {
        return (other.x >= x && other.x <= x + width) &&
               (other.y >= y && other.y <= y + height);
    }
}
