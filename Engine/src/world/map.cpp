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
                std::pair<const Tileset*, const int> tileset = this->parseTilesetElement(element, resourceManager);
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

        // We set the z value of a layer n to 1-1/2^n, so that we have an increasing sequence bounded betwenn 0 and 1
        /*double q = 1;
        for (Entity tileLayer : this->layers)
        {
            // After having created all tile layer components, we add a tileset component to them
            tileLayer.add<TilesetComponent>(tilesetComponent);
            tileLayer.add<PositionComponent>(glm::vec2(0.0));
            tileLayer.add<RenderComponent>(this->width * this->tileWidth, this->height * this->tileHeight, 1 - q);
            q *= 0.5;
        }*/

        return true;
    }

    void Map::unloadResource() {}

    std::pair<const Tileset*, const int> Map::parseTilesetElement(const tinyxml2::XMLElement *element, ResourceManager &resourceManager)
    {
        int firstGid = element->UnsignedAttribute("firstgid");

        std::string tilesetSource = element->Attribute("source");
        std::filesystem::path tilesetPath = std::filesystem::canonical(this->resourcePath.parent_path() / tilesetSource);
        const Tileset *tileset = resourceManager.loadResource<Tileset>(this->resourceName + "tileset", tilesetPath);

        return {tileset, firstGid};
    }

    std::unique_ptr<const TileLayer> Map::parseTileLayerElement(const tinyxml2::XMLElement *element, ResourceManager &resourceManager)
    {
        std::unique_ptr<TileLayer> layer = std::make_unique<TileLayer>();;
        bool result = layer->loadFromXMLElement(element);

        if (result)
            return layer;
        else
            return nullptr;
    }

    unsigned Map::getWidth() const { return this->width; }
    unsigned Map::getHeight() const { return this->height; }
    const std::vector<std::pair<const Tileset*, const int>> &Map::getTilesets() const { return this->tilesets; }
    const std::vector<std::unique_ptr<const TileLayer>> &Map::getLayers() const { return this->layers; }
} // namespace engine