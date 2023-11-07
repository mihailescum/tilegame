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
        std::vector<Tile> _tiles;
        float _z_index;

    public:
        TileLayer(int width, int height, float z_index) : _width(width), _height(height), _tiles(width * height), _z_index(z_index) {}

        void set_data(std::vector<Tile> data) { _tiles = data; }
        int get_width() const { return _width; }
        int get_height() const { return _height; }
        float get_z_index() const { return _z_index; }
        const std::vector<Tile> &get_tiles() const { return _tiles; }
    };
}