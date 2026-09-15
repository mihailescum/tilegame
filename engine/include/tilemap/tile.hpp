#pragma once

#include <string>
#include <memory>

#include "tileson/tileson.hpp"

#include "core/shape.hpp"

namespace engine::tilemap
{
    class Tileset;

    /**
     * @brief A single tile definition belonging to a Tileset: its local id, custom
     * Tiled class/properties, and an optional collision shape parsed from that
     * tile's object layer in Tiled.
     */
    struct Tile
    {
        // Zero based ID indicating the position of the tile in the tileset. 0 = top left.
        // A value of -1 corresponds to an invalid tile.
        int id;
        const Tileset *tileset;
        std::string class_type;
        tson::PropertyCollection properties;

        std::unique_ptr<const Shape> collision_shape; ///< Null if the tile has no collision object defined in Tiled.

        Tile() : id(-1), tileset(nullptr), class_type(""), properties(), collision_shape() {}
    };
} // namespace engine::tilemap
