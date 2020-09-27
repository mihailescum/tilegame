#pragma once

#include "core/spritebatch.hpp"
#include "core/texture2D.hpp"
#include "core/camera.hpp"

#include "map/tilesetinfo.hpp"

namespace engine
{
    class Map;

    class Renderer
    {
    private:
        const Map &map;
        const TilesetInfo *tilesetInfo;
        unsigned mapWidth;
        unsigned mapHeight;

    public:
        Renderer(const Map &map) : map(map) {}
        
        void draw(SpriteBatch &spriteBatch, const Camera &camera) const;
        void initialize();
    };
} // namespace engine
