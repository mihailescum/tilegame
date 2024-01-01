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
            const engine::Texture2D *texture;
            engine::Rectangle destination_rect;
            engine::Rectangle source_rect;
            const engine::Shape *collision_shape;

            TileData() : TileData(nullptr, engine::Rectangle::EMPTY, engine::Rectangle::EMPTY) {}
            TileData(const engine::Texture2D *texture, const engine::Rectangle &destination_rect, const engine::Rectangle &source_rect) : texture(texture), destination_rect(destination_rect), source_rect(source_rect), collision_shape(nullptr) {}
        };

        std::reference_wrapper<const engine::tilemap::TileLayer> layer;
        std::vector<TileData> tile_data;

        const engine::tilemap::TileLayer &operator()() const { return layer.get(); }
    };
} // namespace tilegame
