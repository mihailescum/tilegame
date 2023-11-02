#include "mapsystem.hpp"

#include "components/renderable2d.hpp"
#include "components/ordering.hpp"
#include "components/transform.hpp"
#include "components/tilemap.hpp"
#include "components/tilelayer.hpp"
#include "components/tileset.hpp"
#include "components/scenenode.hpp"

namespace tilegame::systems
{
    MapSystem::MapSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    const entt::entity MapSystem::create_map_entity_from_file(const std::string name, const std::string path)
    {
        auto &resource_manager = _scene.get_game().get_resource_manager();
        const engine::tilemap::TileMap *map_ptr = resource_manager.load_resource<engine::tilemap::TileMap>(name, path);
        if (map_ptr)
        {
            const engine::tilemap::TileMap &map = *map_ptr;

            const auto map_entity = _registry.create();

            _registry.emplace<tilegame::components::TileMap>(map_entity, map);
            _registry.emplace<tilegame::components::Transform>(map_entity, glm::vec2(0.0), glm::vec2(0.0));

            const tilegame::SceneGraphData map_scenedata(map_entity);
            tilegame::SceneGraphNode &map_scenenode = _scene.get_scene_graph_root().add_child(map_scenedata);
            _registry.emplace<tilegame::components::SceneNode>(map_entity, &map_scenenode);

            std::vector<entt::entity> tileset_entities;
            for (const auto &tileset : map.get_tilesets())
            {
                const auto tileset_entity = _registry.create();
                tileset_entities.push_back(tileset_entity);
                _registry.emplace<tilegame::components::Tileset>(tileset_entity, *tileset);
            }

            std::vector<entt::entity> map_children;
            for (const auto &layer : map.get_layers())
            {
                const auto layer_entity = _registry.create();
                map_children.push_back(layer_entity);

                _registry.emplace<tilegame::components::Transform>(layer_entity, glm::vec2(0.0, 0.0), glm::vec2(0.0));
                _registry.emplace<tilegame::components::Ordering>(layer_entity, layer->get_z_index());
                _registry.emplace<tilegame::components::Renderable2D>(layer_entity);

                const tilegame::SceneGraphData layer_scenedata(layer_entity);
                tilegame::SceneGraphNode &layer_scenenode = map_scenenode.add_child(layer_scenedata);
                _registry.emplace<tilegame::components::SceneNode>(layer_entity, &layer_scenenode);

                const auto tiles = layer->get_tiles();
                const auto width = layer->get_width();
                const auto height = layer->get_height();

                std::vector<tilegame::components::TileLayer::TileData> tile_data;
                tile_data.reserve(width * height);
                for (int x = 0; x < width; ++x)
                {
                    for (int y = 0; y < height; ++y)
                    {
                        const auto &tile = tiles[x + width * y];
                        const engine::tilemap::Tileset *tileset = nullptr;
                        for (const auto tileset_entity : tileset_entities)
                        {
                            const engine::tilemap::Tileset &tileset_to_be_checked = _registry.get<tilegame::components::Tileset>(tileset_entity).tileset.get();
                            if (tileset_to_be_checked.has_tile(tile.ID))
                            {
                                tileset = &tileset_to_be_checked;
                                break;
                            }
                        }

                        if (tileset)
                        {
                            const engine::Texture2D &tileset_texture = tileset->get_texture();
                            const auto tile_width = tileset->get_tile_width();
                            const auto tile_height = tileset->get_tile_height();

                            const engine::Rectangle tile_dest_rect(x * tile_width, y * tile_height, tile_width, tile_height);
                            const auto tile_source_rect = tileset->get_source_rect(tile.ID);

                            tilegame::components::TileLayer::TileData data{tileset_texture, tile_dest_rect, tile_source_rect};
                            tile_data.push_back(data);
                        }
                    }
                }
                auto layer_component = _registry.emplace<tilegame::components::TileLayer>(layer_entity, *layer, tile_data);
            }

            return map_entity;
        }
        else
        {
            throw "Could not load map";
        }
    }
}