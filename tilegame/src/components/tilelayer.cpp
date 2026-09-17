#include "tilelayer.hpp"

#include "collider.hpp"

namespace tilegame::components
{
    TileLayer TileLayer::build(const glm::ivec2 &dimensions, const glm::ivec2 &tile_dimensions, const sol::table &cells)
    {
        TileLayer layer;
        layer.dimensions = dimensions;
        layer.tile_dimensions = tile_dimensions;
        layer.tile_data.resize(static_cast<std::size_t>(dimensions.x) * dimensions.y);

        for (std::size_t i = 0; i < layer.tile_data.size(); ++i)
        {
            const sol::optional<sol::table> cell = cells[i + 1];
            if (!cell)
            {
                continue;
            }

            const engine::Texture2D *texture = (*cell)["texture"];
            const engine::Texture2D *luminosity = (*cell)["luminosity"];
            const engine::Rectangle destination_rect = (*cell)["destination"];
            const engine::Rectangle source_rect = (*cell)["source"];

            TileData data(engine::Texture2DContainer<2>{texture, luminosity}, destination_rect, source_rect);

            const sol::optional<sol::table> shape = (*cell)["shape"];
            if (shape)
            {
                data.collision_shape = Collider::make_shape(*shape);
            }

            layer.tile_data[i] = std::move(data);
        }

        return layer;
    }
} // namespace tilegame::components
