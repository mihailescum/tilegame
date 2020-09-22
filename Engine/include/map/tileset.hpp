#ifndef __ENGINE_TILESET_H__
#define __ENGINE_TILESET_H__

#include <string>
#include <vector>

#include "map/tile.hpp"
#include "core/texture2D.hpp"

namespace engine
{
    class Tileset
    {
        private:
            int firstGID = 0;
            std::vector<Tile> tiles;

            std::string name;
            int tileWidth;
            int tileHeight;
            int tileCount;
            int columns;
            int rows;
            Texture2D texture;

        public:
            Tileset() {}
            ~Tileset() {}
            void loadFromFile(const std::string filename, const std::string folder="");
            const Texture2D &getTexture() const;
    };
} // namespace engine

#endif