#pragma once

#include <memory>

#include "world/tileset.hpp"

namespace engine{
    struct TilesetInfo {
        public:
            TilesetInfo() {}
            TilesetInfo(const TilesetInfo &tilesetInfo) = delete;

            unsigned firstGid;
            Tileset *tileset;
    };
}
