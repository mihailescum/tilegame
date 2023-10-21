#pragma once

#include <vector>

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include "core/resource.hpp"

namespace engine::tilemap
{
    class TileLayer
    {
    private:
        int _width;
        int _height;
        std::vector<tmx::TileLayer::Tile> _tiles;

    public:
        TileLayer(int width, int height) : _width(width), _height(height), _tiles(width * height) {}

        void set_data(std::vector<tmx::TileLayer::Tile> data) { _tiles = data; }
    };
}