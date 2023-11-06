#pragma once

#include <any>

namespace engine::tilemap
{
    struct TileObject
    {
    public:
        int x;
        int y;
        int width;
        int height;
        std::any data;
    };
} // namespace engine::tilemap
