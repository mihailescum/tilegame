#ifndef __ENGINE_MAP_H__
#define __ENGINE_MAP_H__

#include <string>
#include <memory>
#include <vector>
#include <cstdarg>
#include <filesystem>
#include "tinyxml2.h"

#include "core/resource.hpp"

#include "map/tilesetinfo.hpp"
#include "map/tilelayer.hpp"

namespace engine
{
    class Map : public Resource
    {
    private:
        TilesetInfo tilesetInfo;
        unsigned width;
        unsigned height;
        std::vector<std::unique_ptr<TileLayer>> layers;

        void parseTilesetElement(ResourceManager &resourceManager, const tinyxml2::XMLElement *element);

    public:
        Map() {}
        Map(const Map &map) = delete;
        
        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override;

        const TilesetInfo &getTilesetInfo() const;
        unsigned getWidth() const;
        unsigned getHeight() const;
        const std::vector<std::unique_ptr<TileLayer>> &getLayers() const;
    };
} // namespace engine

#endif