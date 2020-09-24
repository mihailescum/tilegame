#ifndef __ENGINE_MAP_H__
#define __ENGINE_MAP_H__

#include <string>
#include <memory>
#include <vector>
#include "tinyxml2.h"

#include "map/tilesetinfo.hpp"
#include "map/tilelayer.hpp"

namespace engine
{
    class Map
    {
    private:
        TilesetInfo tilesetInfo;
        unsigned width;
        unsigned height;
        std::vector<std::unique_ptr<TileLayer>> layers;

        void parseTilesetElement(const std::string &path, const tinyxml2::XMLElement *element);

    public:
        Map() {}
        Map(const Map &map) = delete;
        
        void loadFromFile(const std::string &path, const std::string &filename);

        const TilesetInfo &getTilesetInfo() const;
        unsigned getWidth() const;
        unsigned getHeight() const;
        const std::vector<std::unique_ptr<TileLayer>> &getLayers() const;
    };
} // namespace engine

#endif