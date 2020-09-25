#include "map/map.hpp"

#include <cstdarg>
#include <filesystem>
#include "tinyxml2.h"

#include "core/resourcemanager.hpp"

#include "map/tileset.hpp"

namespace engine
{
    bool Map::loadResource(ResourceManager &resourceManager, const std::string &filename, va_list args)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(filename.c_str());
        std::filesystem::path directory = std::filesystem::path(filename).parent_path();

        // Root element
        tinyxml2::XMLElement *root = doc.FirstChildElement();
        this->width = root->UnsignedAttribute("width");
        this->height = root->UnsignedAttribute("height");

        // All other nodes
        const tinyxml2::XMLElement *element = root->FirstChildElement();
        while (element)
        {
            // Tileset data
            std::string value(element->Value());
            if (value == "tileset")
            {
                this->parseTilesetElement(resourceManager, directory, element);
            }
            else if (value == "layer")
            {
                std::unique_ptr<TileLayer> layer = std::make_unique<TileLayer>();
                layer->loadFromXMLElement(element);

                layers.push_back(std::move(layer));
            }

            element = element->NextSiblingElement();
        }

        return true;
    }

    void Map::unloadResource() {}

    void Map::parseTilesetElement(ResourceManager &resourceManager, const std::filesystem::path &directory, const tinyxml2::XMLElement *element)
    {
        int firstGid = element->UnsignedAttribute("firstgid");

        std::string tilesetSource = element->Attribute("source");
        std::filesystem::path tilesetPath = std::filesystem::canonical(std::filesystem::absolute(directory) / tilesetSource);

        this->tilesetInfo.firstGid = firstGid;
        this->tilesetInfo.tileset = resourceManager.loadResource<Tileset>(this->resourceName + "tileset", tilesetPath);
    }

    unsigned Map::getWidth() const { return this->width; }
    unsigned Map::getHeight() const { return this->height; }
    const TilesetInfo &Map::getTilesetInfo() const { return this->tilesetInfo; }
    const std::vector<std::unique_ptr<TileLayer>> &Map::getLayers() const { return this->layers; }
} // namespace engine