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

    bool Map::load_resource(ResourceManager &_resource_manager, va_list args)
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
        for (const nlohmann::json &tilesetDocument : tilesetsDocument)
        {
            std::pair<const Tileset *, const int> tileset = this->parseTilesetDocument(tilesetDocument, _resource_manager);
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
        for (const nlohmann::json &layerDocument : layersDocument)
        {
            std::string layertype = layerDocument.at("type");

            if (layertype == "tilelayer")
            {
                std::unique_ptr<const TileLayer> layer = this->parseTileLayerDocument(layerDocument, _resource_manager);
                if (layer)
                    layers.push_back(std::move(layer));
                else
                    return false;
            }
            else if (layertype == "objectgroup")
            {
                this->parseObjectLayerDocument(layerDocument, _resource_manager);
            }
        }
        if (this->layers.empty())
        {
            Log::w("No tile layers loaded (file: ", this->resourcePath, ").");
        }

        return true;
    }

    void Map::unload_resource() {}

    std::pair<const Tileset *, const int> Map::parseTilesetDocument(const nlohmann::json &document, ResourceManager &_resource_manager)
    {
        int firstGid = document.value("firstgid", 0);
        if (firstGid <= 0)
            return {nullptr, 0};

        std::string tilesetSource = document.value("source", "");
        if (tilesetSource.empty())
            return {nullptr, 0};

        std::filesystem::path tilesetPath = std::filesystem::canonical(this->resourcePath.parent_path() / tilesetSource);
        const Tileset *tileset = _resource_manager.load_resource<Tileset>(this->_resource_name + "tileset", tilesetPath);

        return {tileset, firstGid};
    }

    std::unique_ptr<const TileLayer> Map::parseTileLayerDocument(const nlohmann::json &document, ResourceManager &_resource_manager)
    {
        std::unique_ptr<TileLayer> layer = std::make_unique<TileLayer>();
        bool result = layer->loadFromJsonDocument(document);

        if (result)
            return layer;
        else
            return nullptr;
    }

    void Map::parseObjectLayerDocument(const nlohmann::json &document, ResourceManager &_resource_manager)
    {
        const nlohmann::json &objectsDocument = document.at("objects");
        for (const nlohmann::json &object : objectsDocument)
        {
            const std::string type = object.at("type");
            if (type == "NPC")
            {
                NpcObject npc = this->parseNpcDocument(object, _resource_manager);
                this->objects.push_back(std::make_unique<NpcObject>(npc));
            }
        }
    }

    NpcObject Map::parseNpcDocument(const nlohmann::json &document, ResourceManager &_resource_manager)
    {
        NpcObject result;
        result.x = document.value("x", 0);
        result.y = document.value("y", 0);
        result.width = document.value("width", 0);
        result.height = document.value("height", 0);

        const nlohmann::json &propertiesDocument = document.at("properties");
        std::string objectId;
        for (const nlohmann::json &propertyDocument : propertiesDocument)
        {
            const std::string propertyName = propertyDocument.value("name", "");
            const std::string propertyType = propertyDocument.value("type", "");
            if (propertyName == "object_id")
            {
                if (propertyType == "string")
                    objectId = propertyDocument.value("value", "");
                else if (propertyType == "int")
                    objectId = propertyDocument.value("value", -1);
            }
        }

        result.objectId = objectId;
        return result;
    }

    int Map::getWidth() const { return this->width; }
    int Map::get_height() const { return this->height; }
    int Map::getTileWidth() const { return this->tileWidth; }
    int Map::getTileHeight() const { return this->tileHeight; }
    const std::vector<std::pair<const Tileset *, const int>> &Map::getTilesets() const { return this->tilesets; }
    const std::vector<std::unique_ptr<const TileLayer>> &Map::getLayers() const { return this->layers; }
    const std::vector<std::unique_ptr<const MapObject>> &Map::getObjects() const { return this->objects; }
} // namespace engine