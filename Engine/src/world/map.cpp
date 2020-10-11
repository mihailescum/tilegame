#include "world/map.hpp"

#include <cstdarg>
#include <filesystem>
#include <tinyxml2.h>

#include "core/resourcemanager.hpp"
#include "scene/scene.hpp"
#include "world/objectlayer.hpp"
#include "world/tileset.hpp"
#include "world/tilesetcomponent.hpp"
#include "world/tilelayercomponent.hpp"

namespace engine
{
    bool Map::loadResource(ResourceManager &resourceManager, va_list args)
    {
        Scene *scene(va_arg(args, Scene *));

        tinyxml2::XMLDocument doc;
        doc.LoadFile(this->resourcePath.c_str());

        // Root element
        const tinyxml2::XMLElement *root = doc.FirstChildElement();
        this->width = root->UnsignedAttribute("width");
        this->height = root->UnsignedAttribute("height");

        // TODO: Add support for multiple tilesets through an entt::hirarchy model
        TilesetComponent tilesetComponent;

        // All other nodes
        const tinyxml2::XMLElement *element = root->FirstChildElement();
        while (element)
        {
            std::string value = element->Value();
            if (value == "tileset") 
            {
                tilesetComponent = this->parseTilesetElement(resourceManager, element);
            }
            else if (value == "layer")
            {
                Entity entity = scene->createEntity();
                entity.add<TileLayerComponent>(TileLayerComponent::loadFromXMLElement(element));
                layers.push_back(entity);
            }
            else if (value == "objectgroup")
            {
                //std::unique_ptr<ObjectLayer> layer = std::make_unique<ObjectLayer>();
                //layer->loadFromXMLElement(element);

                //layers.push_back(std::move(layer));
            }

            element = element->NextSiblingElement();
        }

        // After having created all tile layer components, we add a tileset component to them
        for (Entity tileLayer : this->layers)
        {
            tileLayer.add<TilesetComponent>(tilesetComponent);
        }

        return true;
    }

    void Map::unloadResource() {}

    TilesetComponent Map::parseTilesetElement(ResourceManager &resourceManager, const tinyxml2::XMLElement *element)
    {
        TilesetComponent result;

        int firstGid = element->UnsignedAttribute("firstgid");

        std::string tilesetSource = element->Attribute("source");
        std::filesystem::path tilesetPath = std::filesystem::canonical(this->resourcePath.parent_path() / tilesetSource);

        result.firstGid = firstGid;
        result.tileset = resourceManager.loadResource<Tileset>(this->resourceName + "tileset", tilesetPath);

        return result;
    }

    unsigned Map::getWidth() const { return this->width; }
    unsigned Map::getHeight() const { return this->height; }
    const std::vector<Entity> &Map::getLayers() const { return this->layers; }
} // namespace engine