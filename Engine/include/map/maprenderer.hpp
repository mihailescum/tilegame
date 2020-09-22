#ifndef __ENGINE_MAPRENDERER_H__
#define __ENGINE_MAPRENDERER_H__

#include "core/spritebatch.hpp"
#include "core/texture2D.hpp"
#include "map/tilesetinfo.hpp"

namespace engine
{
    class Map;

    class MapRenderer
    {
    private:
        const Map &map;
        const TilesetInfo *tilesetInfo;
        unsigned mapWidth;
        unsigned mapHeight;

    public:
        MapRenderer(const Map &map) : map(map) {}
        ~MapRenderer() {}
        void draw(SpriteBatch &spriteBatch) const;
        void initialize();
    };
} // namespace engine

#endif