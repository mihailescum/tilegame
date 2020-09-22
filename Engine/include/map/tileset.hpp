#ifndef __ENGINE_TILESET_H__
#define __ENGINE_TILESET_H__

#include <string>
#include <vector>

#include "tinyxml2.h"

#include "tile.hpp"

namespace engine
{
    class Tileset
    {
        private:
            int firstGID = 0;
            std::vector<Tile> tiles;
        public:
            Tileset() {}
            ~Tileset() {}
            void loadFromFile(const std::string filename);
    };
} // namespace engine

#endif