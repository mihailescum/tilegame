#ifndef __ENGINE_MAPRENDERER_H__
#define __ENGINE_MAPRENDERER_H__

#include "core/spritebatch.hpp"

namespace engine
{
    class Map;

    class MapRenderer
    {
    private:
        const Map &map;

    public:
        MapRenderer(const Map &map) : map(map) {}
        ~MapRenderer() {}
        void Draw(SpriteBatch &spriteBatch) const;
    };
} // namespace engine

#endif