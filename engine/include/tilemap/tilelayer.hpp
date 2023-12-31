#pragma once

#include <vector>

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include "core/resource.hpp"
#include "tilemap/tile.hpp"

namespace engine::tilemap
{
    class TileLayer
    {
    private:
        int _width;
        int _height;
        std::vector<int> _tiles;
        float _z_index;

    public:
        TileLayer(int width, int height, float z_index) : _width(width), _height(height), _tiles(width * height), _z_index(z_index) {}

        int width() const { return _width; }
        int height() const { return _height; }
        float z_index() const { return _z_index; }
        void data(std::vector<int> data) { _tiles = data; }
        const std::vector<int> &tiles() const { return _tiles; }
    };
}