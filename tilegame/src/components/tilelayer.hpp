#pragma once

#include <functional>
#include <vector>

#include "engine.hpp"

namespace tilegame::components
{
    struct TileLayer
    {
        struct TileData
        {
            std::reference_wrapper<const engine::Texture2D> texture;
            engine::Rectangle destination_rect;
            engine::Rectangle source_rect;
            const engine::Shape *collision_shape;
        };

        std::reference_wrapper<const engine::tilemap::TileLayer> layer;
        std::vector<TileData> tile_data;
    };
} // namespace tilegame
