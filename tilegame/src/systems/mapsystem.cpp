#include "mapsystem.hpp"

#include "components/renderable2d.hpp"
#include "components/renderlayer.hpp"
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
        auto map_ptr = resource_manager.load_resource<engine::tilemap::TileMap>(name, path);
        if (map_ptr)
        {
            auto map = *map_ptr;

            const auto map_entity = _registry.create();

            _registry.emplace<tilegame::components::TileMap>(map_entity, map);
            _registry.emplace<tilegame::components::Transform>(map_entity, glm::vec2(0.0), glm::vec2(0.0));

            tilegame::SceneGraphData map_scenedata(map_entity);
            auto map_scenenode = _scene.get_scene_graph_root().add_child(map_scenedata);
            _registry.emplace<tilegame::components::SceneNode>(map_entity, map_scenenode);

            std::vector<entt::entity> tileset_entities;
            for (const auto &tileset : map.get_tilesets())
            {
                const auto tileset_entity = _registry.create();
                tileset_entities.push_back(tileset_entity);
                _registry.emplace<tilegame::components::Tileset>(tileset_entity, tileset);
            }

            std::vector<entt::entity> map_children;
            for (const auto &layer : map.get_layers())
            {
                const auto layer_entity = _registry.create();
                map_children.push_back(layer_entity);

                _registry.emplace<tilegame::components::TileLayer>(layer_entity, layer);
                _registry.emplace<tilegame::components::Transform>(layer_entity, glm::vec2(0.0, 0.0), glm::vec2(0.0));

                tilegame::SceneGraphData layer_scenedata(layer_entity);
                auto layer_scenenode = map_scenenode->add_child(layer_scenedata);
                _registry.emplace<tilegame::components::SceneNode>(layer_entity, layer_scenenode);

                const auto tiles = layer.get_tiles();
                auto width = layer.get_width();
                auto height = layer.get_height();

                std::vector<entt::entity> layer_children;
                for (int x = 0; x < width; ++x)
                {
                    for (int y = 0; y < height; ++y)
                    {
                        const auto &tile = tiles[x + width * y];
                        for (const auto tileset_entity : tileset_entities)
                        {
                            const auto &tileset = _registry.get<tilegame::components::Tileset>(tileset_entity).tileset.get();
                            if (tileset.has_tile(tile.ID))
                            {
                                auto tile_width = tileset.get_tile_width();
                                auto tile_height = tileset.get_tile_height();
                                const auto &tileset_texture = tileset.get_texture();
                                auto tile_source_rect = tileset.get_source_rect(tile.ID);

                                const auto tile_entity = _registry.create();
                                layer_children.push_back(tile_entity);

                                _registry.emplace<tilegame::components::Transform>(tile_entity, glm::vec2(x * tile_width, y * tile_height), glm::vec2(0.0));
                                _registry.emplace<tilegame::components::Renderable2D>(tile_entity, tileset_texture, tile_source_rect);

                                tilegame::SceneGraphData tile_scenedata(tile_entity);
                                auto tile_scenenode = layer_scenenode->add_child(tile_scenedata);
                                _registry.emplace<tilegame::components::SceneNode>(tile_entity, tile_scenenode);

                                break;
                            }
                        }
                    }
                }
                _registry.emplace<tilegame::components::RenderLayer>(layer_entity, layer.get_z_index(), layer_children);
            }

            return map_entity;
        }
        else
        {
            throw "Could not load map";
        }
    }
}