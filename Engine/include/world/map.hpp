#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <tinyxml2.h>

#include "core/resource.hpp"
#include "scene/entity.hpp"
#include "world/tilelayer.hpp"
#include "world/tileset.hpp"

namespace engine
{
    class Map : public Resource
    {
    private:
        int width;
        int height;
        int tileWidth;
        int tileHeight;
        std::vector<std::unique_ptr<const TileLayer>> layers;
        std::vector<std::pair<const Tileset*, const int>> tilesets;

        std::pair<const Tileset*, const int> parseTilesetElement(const tinyxml2::XMLElement *element, ResourceManager &resourceManager);
        std::unique_ptr<const TileLayer> parseTileLayerElement(const tinyxml2::XMLElement *element, ResourceManager &resourceManager);

    public:
        Map() {}
        
        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override;

        unsigned getWidth() const;
        unsigned getHeight() const;
        const std::vector<std::pair<const Tileset*, const int>> &getTilesets() const;
        const std::vector<std::unique_ptr<const TileLayer>> &getLayers() const;
    };
} // namespace engine
