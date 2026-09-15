#pragma once

#include "tileson/tileson.hpp"

namespace engine::tilemap
{
    /**
     * @brief Wraps a raw Tiled object (from an object group layer), e.g. a spawn
     * point, trigger area or collision shape placed directly on the map rather than
     * on a tile. Consumed by tilegame's own Map/Render systems.
     */
    struct TileObject
    {
    public:
        tson::Object data;

        TileObject() {}
        TileObject(const tson::Object &data) : data(data) {}
    };
} // namespace engine::tilemap
