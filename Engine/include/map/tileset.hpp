#ifndef __ENGINE_TILESET_H__
#define __ENGINE_TILESET_H__

#include <string>
#include <vector>
#include <memory>

#include "map/tile.hpp"
#include "core/texture2D.hpp"

namespace engine
{
    class Tileset
    {
        private:
            std::vector<Tile> tiles;

            std::string name;
            unsigned tileWidth;
            unsigned tileHeight;
            unsigned tileCount;
            unsigned columns;
            unsigned rows;
            std::unique_ptr<Texture2D> texture;

        public:
            Tileset() {}
            Tileset(const Tileset &tileset) = delete;
            ~Tileset() {}

            void loadFromFile(const std::string &path, const std::string &filename);
            const Tile &getTile(const unsigned index) const;

            const Texture2D &getTexture() const;
            unsigned getTileWidth() const;
            unsigned getTileHeight() const;
    };
} // namespace engine

#endif