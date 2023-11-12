#pragma once

#include "tileson/tileson.hpp"

namespace engine::tilemap
{
    struct TileObject
    {
    public:
        tson::Object data;

        TileObject() {}
        TileObject(const tson::Object &data) : data(data) {}
    };
} // namespace engine::tilemap
