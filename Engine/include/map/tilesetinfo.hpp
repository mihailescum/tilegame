#pragma once

#include <memory>

#include "map/tileset.hpp"

namespace engine{
    struct TilesetInfo {
        public:
            TilesetInfo() {}
            TilesetInfo(const TilesetInfo &tilesetInfo) = delete;

            unsigned firstGid;
            Tileset *tileset;
    };
}
