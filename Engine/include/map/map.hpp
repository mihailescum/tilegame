#ifndef __ENGINE_MAP_H__
#define __ENGINE_MAP_H__

#include <string>
#include <memory>
#include <vector>

#include "map/tilesetinfo.hpp"
#include "map/maprenderer.hpp"

namespace engine
{
    class Map
    {
    private:
        TilesetInfo tilesetInfo;
        std::vector<unsigned> data;
        unsigned width;
        unsigned height;

        std::unique_ptr<MapRenderer> mapRenderer;

        void parseCSV(const std::string &csv);

    public:
        Map();
        Map(const Map &map) = delete;
        ~Map() {}
        void loadFromFile(const std::string &path, const std::string &filename);
        void Draw(SpriteBatch &spriteBatch) const;

        unsigned getWidth() const;
        unsigned getHeight() const;
        const TilesetInfo &getTilesetInfo() const;
        const std::vector<unsigned> &getData() const;
    };
} // namespace engine

#endif