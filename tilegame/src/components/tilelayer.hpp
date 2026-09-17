#pragma once

#include <optional>
#include <vector>

#include <glm/glm.hpp>

#include "sol/sol.hpp"

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Renders and collides against one grid layer of a loaded map. Fully self-contained
     * (built once by content/scripts/maploader.lua from the map's raw JSON, via `_load_json` and
     * systems::Script's `_emplace_tilelayer` binding - see build()): `tile_data` gives each
     * cell's draw rects, textures and optional collision shape, which the Render and
     * CollisionDetection systems iterate directly.
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
            /// Absent when the tile has no collision geometry.
            std::optional<engine::ShapeVariant> collision_shape;

            TileData() : TileData(engine::Texture2DContainer<2>(), engine::Rectangle::EMPTY, engine::Rectangle::EMPTY) {}
            TileData(const engine::Texture2DContainer<2> &textures, const engine::Rectangle &destination_rect, const engine::Rectangle &source_rect) : textures(textures), destination_rect(destination_rect), source_rect(source_rect) {}
        };

        glm::ivec2 dimensions;
        glm::ivec2 tile_dimensions;
        std::vector<TileData> tile_data;

        /** @brief Flat index into tile_data for grid cell (x, y), or -1 if the coordinates are outside `dimensions`. */
        int index(int x, int y) const
        {
            if (x >= 0 && x < dimensions.x && y >= 0 && y < dimensions.y)
            {
                return x + dimensions.x * y;
            }
            else
            {
                return -1;
            }
        }

        /// Builds a TileLayer from Lua-supplied dimensions and a 1-based array `cells` of
        /// per-cell tables ({texture, luminosity, destination, source, shape?} - `shape`, if
        /// present, is a descriptor as accepted by Collider::make_shape()), or a nil entry for
        /// an empty cell.
        static TileLayer build(const glm::ivec2 &dimensions, const glm::ivec2 &tile_dimensions, const sol::table &cells);
    };
} // namespace tilegame::components
