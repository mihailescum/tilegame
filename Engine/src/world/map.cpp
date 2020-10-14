#include "world/map.hpp"

#include <cstdarg>
#include <filesystem>
#include <tinyxml2.h>
#include <glm/glm.hpp>

#include "core/resourcemanager.hpp"
#include "world/objectlayer.hpp"
#include "world/tileset.hpp"

namespace engine
{
    bool Map::loadResource(ResourceManager &resourceManager, va_list args)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(this->resourcePath.c_str());

        // Root element
        const tinyxml2::XMLElement *root = doc.FirstChildElement();
        this->width = root->UnsignedAttribute("width");
        this->height = root->UnsignedAttribute("height");
        this->tileWidth = root->UnsignedAttribute("tilewidth");
        this->tileHeight = root->UnsignedAttribute("tileheight");

        // All other nodes
        const tinyxml2::XMLElement *element = root->FirstChildElement();
        while (element)
        {
            std::string value = element->Value();
            if (value == "tileset")
            {
                std::pair<const Tileset *, const int> tileset = this->parseTilesetElement(element, resourceManager);
                if (tileset.first)
                    tilesets.push_back(tileset);
                else
                    return false;
            }
            else if (value == "layer")
            {
                std::unique_ptr<const TileLayer> layer = this->parseTileLayerElement(element, resourceManager);
                if (layer)
                    layers.push_back(std::move(layer));
                else
                    return false;
            }
            else if (value == "objectgroup")
            {
                //std::unique_ptr<ObjectLayer> layer = std::make_unique<ObjectLayer>();
                //layer->loadFromXMLElement(element);

                //layers.push_back(std::move(layer));
            }

            element = element->NextSiblingElement();
        }

        return true;
    }

    void Map::unloadResource() {}

    std::pair<const Tileset *, const int> Map::parseTilesetElement(const tinyxml2::XMLElement *element, ResourceManager &resourceManager)
    {
        int firstGid = element->UnsignedAttribute("firstgid");

        std::string tilesetSource = element->Attribute("source");
        std::filesystem::path tilesetPath = std::filesystem::canonical(this->resourcePath.parent_path() / tilesetSource);
        const Tileset *tileset = resourceManager.loadResource<Tileset>(this->resourceName + "tileset", tilesetPath);

        return {tileset, firstGid};
    }

    std::unique_ptr<const TileLayer> Map::parseTileLayerElement(const tinyxml2::XMLElement *element, ResourceManager &resourceManager)
    {
        std::unique_ptr<TileLayer> layer = std::make_unique<TileLayer>();
        ;
        bool result = layer->loadFromXMLElement(element);

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