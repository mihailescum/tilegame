#pragma once

#include <functional>
#include <vector>

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Renders and collides against one layer of a loaded tilemap. Wraps the engine's
     * `tilemap::TileLayer` asset with a per-tile `tile_data` array giving each cell's draw rects and
     * optional collision shape, which the Render and CollisionDetection systems iterate directly.
     */
    struct TileLayer
    {
        struct TileData
        {
            // textures[0] is the actual texture
            // textures[1] is the luminosity texture
            engine::Texture2DContainer<2> textures;
            engine::Rectangle destination_rect;
            engine::Rectangle source_rect;
            /// Non-owning; null when the tile has no collision geometry.
            const engine::Shape *collision_shape;

            TileData() : TileData(engine::Texture2DContainer<2>(), engine::Rectangle::EMPTY, engine::Rectangle::EMPTY) {}
            TileData(const engine::Texture2DContainer<2> &textures, const engine::Rectangle &destination_rect, const engine::Rectangle &source_rect) : textures(textures), destination_rect(destination_rect), source_rect(source_rect), collision_shape(nullptr) {}
        };

        std::reference_wrapper<const engine::tilemap::TileLayer> layer;
        std::vector<TileData> tile_data;

        const engine::tilemap::TileLayer &operator()() const { return layer.get(); }
    };
} // namespace tilegame
