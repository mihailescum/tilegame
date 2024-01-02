#include "tilemap/tilelayer.hpp"

namespace engine::tilemap
{
    int TileLayer::index(int x, int y) const
    {
        if (x >= 0 && x < _size.x && y >= 0 && y < _size.y)
        {
            return x + _size.x * y;
        }
        else
        {
            return -1;
        }
    }
} // namespace engine::tilemap
