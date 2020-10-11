#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cstdarg>
#include <filesystem>
#include <tinyxml2.h>

#include "core/resource.hpp"
#include "scene/entity.hpp"
#include "world/tilesetcomponent.hpp"
#include "world/tilelayercomponent.hpp"

namespace engine
{
    class Map : public Resource
    {
    private:
        int width;
        int height;
        int tileWidth;
        int tileHeight;
        std::vector<Entity> layers;

        TilesetComponent parseTilesetElement(ResourceManager &resourceManager, const tinyxml2::XMLElement *element);

    public:
        Map() {}
        
        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override;

        unsigned getWidth() const;
        unsigned getHeight() const;
        const std::vector<Entity> &getLayers() const;
    };
} // namespace engine
