#pragma once

#include <glm/glm.hpp>

#include "core/spritebatch.hpp"
#include "core/texture2D.hpp"

namespace engine
{
    class Map;

    class Renderer
    {
    private:
        const Map &map;
        unsigned mapWidth;
        unsigned mapHeight;

    public:
        Renderer(const Map &map) : map(map) {}
        
        void draw(SpriteBatch &spriteBatch, const glm::mat4 &transform) const;
        void initialize();
    };
} // namespace engine
