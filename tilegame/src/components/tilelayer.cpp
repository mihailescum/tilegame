#include "tilelayer.hpp"

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

#include "collider.hpp"

namespace tilegame::components
{
    void TileLayer::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<TileLayer>();

        lua.new_usertype<TileLayer>(
            "_TileLayer",
            "type_id", &entt::type_hash<TileLayer>::value,
            sol::call_constructor,
            sol::factories(
                [](const glm::vec2 &tile_dimensions, const sol::table &cells)
                {
                    TileLayer layer;
                    layer.tile_dimensions = glm::ivec2(tile_dimensions.x, tile_dimensions.y);
                    layer.tile_data.resize(cells.size());

                    for (const auto &[key, value] : cells)
                    {
                        if (!value.is<sol::table>())
                        {
                            continue;
                        }
                        const sol::table cell = value.as<sol::table>();

                        const engine::Texture2D *texture = cell["texture"];
                        const engine::Texture2D *luminosity = cell["luminosity"];
                        const engine::Rectangle destination_rect = cell["destination"];
                        const engine::Rectangle source_rect = cell["source"];

                        TileData data(engine::Texture2DContainer<2>{texture, luminosity}, destination_rect, source_rect);
                        data.depth = cell["depth"];
                        data.reference_y = cell["reference_y"];

                        const sol::optional<sol::table> shape = cell["shape"];
                        if (shape)
                        {
                            data.collision_shape = Collider::make_shape(*shape);
                        }

                        const std::size_t index = key.as<std::size_t>() - 1;
                        layer.tile_data[index] = std::move(data);
                    }

                    return layer;
                }));
    }
} // namespace tilegame::components
