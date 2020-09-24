#ifndef __ENGINE_TILESETINFO_H__
#define __ENGINE_TILESETINFO_H__

#include <memory>

#include "map/tileset.hpp"

namespace engine{
    struct TilesetInfo {
        public:
            TilesetInfo() {}
            TilesetInfo(const TilesetInfo &tilesetInfo) = delete;

            unsigned firstGid;
            std::unique_ptr<Tileset> tileset;
    };
}

#endif