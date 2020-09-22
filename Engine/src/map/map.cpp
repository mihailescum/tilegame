#include "map/map.hpp"

#include "tinyxml2.h"

#include "map/tileset.hpp"

namespace engine
{
    Map::Map()
    {
    }

    void Map::loadFromFile(const std::string &path, const std::string &filename)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile((path + filename).c_str());

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
                this->parseTilesetElement(path, element);
            }
            else if (value == "layer")
            {
                std::unique_ptr<TileLayer> layer(new TileLayer());
                layer->loadFromXMLElement(element);

                layers.push_back(std::move(layer));
            }

            element = element->NextSiblingElement();
        }
    }

    void Map::parseTilesetElement(const std::string &path, const tinyxml2::XMLElement *element)
    {
        int firstGid = element->UnsignedAttribute("firstgid");

        std::string tilesetSource = element->Attribute("source");
        std::unique_ptr<Tileset> tileset(new Tileset());
        tileset->loadFromFile(path, tilesetSource);

        this->tilesetInfo.firstGid = firstGid;
        this->tilesetInfo.tileset = std::move(tileset);
    }

    unsigned Map::getWidth() const { return this->width; }
    unsigned Map::getHeight() const { return this->height; }
    const TilesetInfo &Map::getTilesetInfo() const { return this->tilesetInfo; }
    const std::vector<std::unique_ptr<TileLayer>> &Map::getLayers() const { return this->layers; }
} // namespace engine