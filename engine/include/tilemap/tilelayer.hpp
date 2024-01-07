#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "core/resource.hpp"
#include "tilemap/tile.hpp"

namespace engine::tilemap
{
    class TileLayer
    {
    private:
        glm::ivec2 _dimensions;
        glm::ivec2 _tile_dimensions;
        std::vector<int> _tiles;
        float _z_index;

    public:
        TileLayer(glm::ivec2 size, glm::ivec2 tile_dimensions, float z_index) : _dimensions(size),
                                                                          _tile_dimensions(tile_dimensions),
                                                                          _tiles(size.x * size.y),
                                                                          _z_index(z_index) {}

        const glm::ivec2 &dimensions() const { return _dimensions; }
        const glm::ivec2 &tile_dimensions() const { return _tile_dimensions; }
        float z_index() const { return _z_index; }
        void data(std::vector<int> data) { _tiles = data; }
        const std::vector<int> &tiles() const { return _tiles; }

        int index(int x, int y) const;
    };
}