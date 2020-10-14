#pragma once

#include <memory>

#include "world/tileset.hpp"

namespace engine
{
    struct TilesetComponent
    {
    public:
        TilesetComponent() {}
        TilesetComponent(const TilesetComponent &tilesetInfo) = default;
        TilesetComponent(const unsigned firstGid, const Tileset *tileset) : firstGid(firstGid), tileset(tileset) {}

        unsigned firstGid;
        const Tileset *tileset;
    };
} // namespace engine
