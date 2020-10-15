#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "nlohmann/json.hpp"

#include "core/resource.hpp"
#include "scene/entity.hpp"
#include "world/tilelayer.hpp"
#include "world/tileset.hpp"
#include "world/mapobjects.hpp"

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
        std::vector<std::unique_ptr<const MapObject>> objects;

        const std::unique_ptr<nlohmann::json> loadJsonDocument() const;
        std::pair<const Tileset*, const int> parseTilesetDocument(const nlohmann::json &document, ResourceManager &resourceManager);
        std::unique_ptr<const TileLayer> parseTileLayerDocument(const nlohmann::json &document, ResourceManager &resourceManager);
        void parseObjectLayerDocument(const nlohmann::json &document, ResourceManager &resourceManager);
        NpcObject parseNpcDocument(const nlohmann::json &document, ResourceManager &resourceManager);

    public:
        Map() {}
        
        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override;

        int getWidth() const;
        int getHeight() const;
        int getTileWidth() const;
        int getTileHeight() const;
        const std::vector<std::pair<const Tileset*, const int>> &getTilesets() const;
        const std::vector<std::unique_ptr<const TileLayer>> &getLayers() const;
        const std::vector<std::unique_ptr<const MapObject>> &getObjects() const;
    };
} // namespace engine
