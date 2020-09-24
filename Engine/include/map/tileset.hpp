#ifndef __ENGINE_TILESET_H__
#define __ENGINE_TILESET_H__

#include <string>
#include <vector>
#include <memory>
#include <cstdarg>

#include "core/resource.hpp"
#include "core/texture2D.hpp"

#include "map/tile.hpp"

namespace engine
{
    class Tileset : public Resource
    {
    private:
        std::vector<Tile> tiles;

        std::string name;
        unsigned tileWidth;
        unsigned tileHeight;
        unsigned tileCount;
        unsigned columns;
        unsigned rows;
        Texture2D *texture;

    public:
        Tileset() {}
        Tileset(const Tileset &tileset) = delete;

        virtual bool loadResource(ResourceManager &resourceManager, const std::string &filename, va_list args) override;
        virtual void unloadResource() override;
        const Tile &getTile(const unsigned index) const;

        const Texture2D &getTexture() const;
        unsigned getTileWidth() const;
        unsigned getTileHeight() const;
    };
} // namespace engine

#endif