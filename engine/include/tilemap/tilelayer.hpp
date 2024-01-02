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
        glm::ivec2 _size;
        glm::ivec2 _tile_size;
        std::vector<int> _tiles;
        float _z_index;

    public:
        TileLayer(glm::ivec2 size, glm::ivec2 tile_size, float z_index) : _size(size),
                                                                          _tile_size(tile_size),
                                                                          _tiles(size.x * size.y),
                                                                          _z_index(z_index) {}

        const glm::ivec2 &size() const { return _size; }
        const glm::ivec2 &tile_size() const { return _tile_size; }
        float z_index() const { return _z_index; }
        void data(std::vector<int> data) { _tiles = data; }
        const std::vector<int> &tiles() const { return _tiles; }

        int index(int x, int y) const;
    };
}