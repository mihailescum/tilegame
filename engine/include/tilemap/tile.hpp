#pragma once

#include <string>
#include <memory>

#include "core/shape.hpp"

namespace engine::tilemap
{
    class Tileset;

    struct Tile
    {
        int id;
        const Tileset *tileset;
        std::string class_type;
        tson::PropertyCollection properties;

        std::unique_ptr<const Shape> collision_shape;
    };
} // namespace engine::tilemap
