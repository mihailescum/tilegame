#include "map/map.hpp"

#include <cstdarg>
#include <filesystem>
#include <tinyxml2.h>

#include "core/resourcemanager.hpp"
#include "map/objectlayer.hpp"

#include "map/tileset.hpp"

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

        // All other nodes
        const tinyxml2::XMLElement *element = root->FirstChildElement();
        while (element)
        {
            // Tileset data
            std::string value = element->Value();
            if (value == "tileset")
            {
                this->parseTilesetElement(resourceManager, element);
            }
            else if (value == "layer")
            {
                std::unique_ptr<TileLayer> layer = std::make_unique<TileLayer>();
                layer->loadFromXMLElement(element);

                layers.push_back(std::move(layer));
            }
            else if (value == "objectgroup") {
                std::unique_ptr<ObjectLayer> layer = std::make_unique<ObjectLayer>();
                layer->loadFromXMLElement(element);

                //layers.push_back(std::move(layer));
            }

            element = element->NextSiblingElement();
        }

        return true;
    }

    void Map::unloadResource() {}

    void Map::parseTilesetElement(ResourceManager &resourceManager, const tinyxml2::XMLElement *element)
    {
        int firstGid = element->UnsignedAttribute("firstgid");

        std::string tilesetSource = element->Attribute("source");
        std::filesystem::path tilesetPath = std::filesystem::canonical(this->resourcePath.parent_path() / tilesetSource);

        this->tilesetInfo.firstGid = firstGid;
        this->tilesetInfo.tileset = resourceManager.loadResource<Tileset>(this->resourceName + "tileset", tilesetPath);
    }

    unsigned Map::getWidth() const { return this->width; }
    unsigned Map::getHeight() const { return this->height; }
    const TilesetInfo &Map::getTilesetInfo() const { return this->tilesetInfo; }
    const std::vector<std::unique_ptr<TileLayer>> &Map::getLayers() const { return this->layers; }
} // namespace engine