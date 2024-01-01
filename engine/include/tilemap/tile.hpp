#pragma once

#include <string>
#include <memory>

#include "core/shape.hpp"

namespace engine::tilemap
{
    class Tileset;

    struct Tile
    {
        // Zero based ID indicating the position of the tile in the tileset. 0 = top left.
        // A value of -1 corresponds to an invalid tile.
        int id;
        const Tileset *tileset;
        std::string class_type;
        tson::PropertyCollection properties;

        std::unique_ptr<const Shape> collision_shape;

        Tile() : id(-1) {}
    };
} // namespace engine::tilemap
