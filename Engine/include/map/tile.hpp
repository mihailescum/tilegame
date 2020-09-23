#ifndef __ENGINE_TILE_H__
#define __ENGINE_TILE_H__

#include "core/rectangle.hpp"

namespace engine
{
    struct Tile
    {
        public:
            Tile() {}
            ~Tile() {}
            
            Rectangle sourceRect;
    };
} // namespace engine

#endif