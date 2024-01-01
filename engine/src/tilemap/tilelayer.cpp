#include "tilemap/tilelayer.hpp"

namespace engine::tilemap
{
    int TileLayer::index(int x, int y) const
    {
        if (x >= 0 && x < _width && y >= 0 && y < _height)
        {
            return x + _width * y;
        }
        else
        {
            return -1;
        }
    }
} // namespace engine::tilemap
