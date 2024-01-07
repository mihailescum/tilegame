#include "tilemap/tilelayer.hpp"

namespace engine::tilemap
{
    int TileLayer::index(int x, int y) const
    {
        if (x >= 0 && x < _dimensions.x && y >= 0 && y < _dimensions.y)
        {
            return x + _dimensions.x * y;
        }
        else
        {
            return -1;
        }
    }
} // namespace engine::tilemap
