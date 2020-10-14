#include "world/map.hpp"

#include <cstdarg>
#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <sstream>

#include "core/resourcemanager.hpp"
#include "core/log.hpp"
#include "world/objectlayer.hpp"
#include "world/tileset.hpp"

namespace engine
{
    const std::unique_ptr<nlohmann::json> Map::loadJsonDocument() const
    {
        std::unique_ptr<nlohmann::json> result = std::make_unique<nlohmann::json>();

        std::ifstream fileStream(this->resourcePath, std::ifstream::in);
        if (!fileStream.is_open())
        {
            return nullptr;
        }
        else
        {
            fileStream >> *result;
        }
        return result;
    }

    bool Map::loadResource(ResourceManager &resourceManager, va_list args)
    {
        std::unique_ptr<nlohmann::json> jsonDocument = this->loadJsonDocument();
        if (!jsonDocument)
        {
            Log::e("File could not be loaded (file: ", this->resourcePath, ").");
            return false;
        }

        this->width = jsonDocument->value("width", 0);
        if (this->width <= 0)
        {
            Log::e("Map width (", this->width, ") has to be greater than zero (file: ", this->resourcePath, ").");
            return false;
        }

        this->height = jsonDocument->value("height", 0);
        if (this->height <= 0)
        {
            Log::e("Map height (", this->height, ") has to be greater than zero (file: ", this->resourcePath, ").");
            return false;
        }

        this->tileWidth = jsonDocument->value("tilewidth", 0);
        if (this->tileWidth <= 0)
        {
            Log::e("Tile width (", this->tileWidth, ") has to be greater than zero (file: ", this->resourcePath, ").");
            return false;
        }

        this->tileHeight = jsonDocument->value("tileheight", 0);
        if (this->tileHeight <= 0)
        {
            Log::e("Tile hidth (", this->tileHeight, ") has to be greater than zero (file: ", this->resourcePath, ").");
            return false;
        }

        const nlohmann::json &tilesetsDocument = jsonDocument->at("tilesets");
        for (nlohmann::json tilesetDocument : tilesetsDocument)
        {
            std::pair<const Tileset *, const int> tileset = this->parseTilesetDocument(tilesetDocument, resourceManager);
            if (tileset.first)
                tilesets.push_back(tileset);
            else
                return false;
        }
        if (this->tilesets.empty())
        {
            Log::w("No tilesets loaded (file: ", this->resourcePath, ").");
        }

        const nlohmann::json &layersDocument = jsonDocument->at("layers");
        for (nlohmann::json layerDocument : layersDocument)
        {
            std::string layertype = layerDocument.at("type");

            if (layertype == "tilelayer")
            {
                std::unique_ptr<const TileLayer> layer = this->parseTileLayerDocument(layerDocument, resourceManager);
                if (layer)
                    layers.push_back(std::move(layer));
                else
                    return false;
            }
        }
        if (this->layers.empty())
        {
            Log::w("No tile layers loaded (file: ", this->resourcePath, ").");
        }

        return true;
    }

    void Map::unloadResource() {}

    std::pair<const Tileset *, const int> Map::parseTilesetDocument(const nlohmann::json &document, ResourceManager &resourceManager)
    {
        int firstGid = document.value("firstgid", 0);
        if (firstGid <= 0)
            return {nullptr, 0};

        std::string tilesetSource = document.value("source", "");
        if (tilesetSource.empty())
            return {nullptr, 0};

        std::filesystem::path tilesetPath = std::filesystem::canonical(this->resourcePath.parent_path() / tilesetSource);
        const Tileset *tileset = resourceManager.loadResource<Tileset>(this->resourceName + "tileset", tilesetPath);

        return {tileset, firstGid};
    }

    std::unique_ptr<const TileLayer> Map::parseTileLayerDocument(const nlohmann::json &document, ResourceManager &resourceManager)
    {
        std::unique_ptr<TileLayer> layer = std::make_unique<TileLayer>();
        bool result = layer->loadFromJsonDocument(document);

        if (result)
            return layer;
        else
            return nullptr;
    }

    int Map::getWidth() const { return this->width; }
    int Map::getHeight() const { return this->height; }
    int Map::getTileWidth() const { return this->tileWidth; }
    int Map::getTileHeight() const { return this->tileHeight; }
    const std::vector<std::pair<const Tileset *, const int>> &Map::getTilesets() const { return this->tilesets; }
    const std::vector<std::unique_ptr<const TileLayer>> &Map::getLayers() const { return this->layers; }
} // namespace engine