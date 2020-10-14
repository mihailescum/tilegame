#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdarg>
#include "nlohmann/json.hpp"

#include "core/resource.hpp"
#include "core/texture2D.hpp"

#include "world/tile.hpp"

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

        const std::unique_ptr<nlohmann::json> loadJsonDocument() const;

    public:
        Tileset() {}
        Tileset(const Tileset &tileset) = delete;

        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override;
        const Tile &getTile(const unsigned index) const;

        const Texture2D &getTexture() const;
        unsigned getTileWidth() const;
        unsigned getTileHeight() const;
    };
} // namespace engine
