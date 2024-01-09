#include "map.hpp"

#include "components/renderable2d.hpp"
#include "components/ordering.hpp"
#include "components/transform.hpp"
#include "components/tilemap.hpp"
#include "components/tilelayer.hpp"
#include "components/tileset.hpp"
#include "components/scenenode.hpp"
#include "components/animation.hpp"
#include "components/sprite.hpp"
#include "components/scriptloader.hpp"
#include "components/collider.hpp"

namespace tilegame::systems
{
    Map::Map(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Map::load_content()
    {
        auto &resource_manager = _scene.game().resource_manager();
        const engine::tilemap::TileMap &map = *resource_manager.load_resource<engine::tilemap::TileMap>("map1", "content/maps/map1.tmj");
        create_map_entity(map);
    }

    const entt::entity Map::create_map_entity(const engine::tilemap::TileMap &map)
    {
        const auto entity = _registry.create();
        _registry.emplace<components::TileMap>(entity, std::ref(map));
        _registry.emplace<components::Transform>(entity, glm::vec2(0.0));

        // const tilegame::SceneGraphData map_scenedata(entity);
        // tilegame::SceneGraphNode &map_scenenode = _scene.scene_graph_root().add_child(map_scenedata);
        //_registry.emplace<components::SceneNode>(entity, &map_scenenode);

        std::vector<entt::entity> tileset_entities;
        for (const auto &[first_gid, last_gid, tileset] : map.tilesets())
        {
            const auto tileset_entity = create_tileset_entity(*tileset);
            tileset_entities.push_back(tileset_entity);
        }

        for (const auto &layer : map.layers())
        {
            const auto layer_entity = create_layer_entity(*layer, map);

            // const tilegame::SceneGraphData layer_scenedata(layer_entity);
            // tilegame::SceneGraphNode &layer_scenenode = map_scenenode.add_child(layer_scenedata);
            //_registry.emplace<components::SceneNode>(layer_entity, &layer_scenenode);
        }

        for (const auto &object : map.objects())
        {
            const auto &data = object->data;
            if (data.getGid() > 0) // parse a sprite
            {
                const auto sprite_entity = create_sprite_entity(*object, map);

                // const tilegame::SceneGraphData sprite_scenedata(sprite_entity);
                // tilegame::SceneGraphNode &sprite_scenenode = map_scenenode.add_child(sprite_scenedata);
                //_registry.emplace<components::SceneNode>(sprite_entity, &sprite_scenenode);
            }
            else
            {
                // TODO
            }
        }

        return entity;
    }

    const entt::entity Map::create_layer_entity(const engine::tilemap::TileLayer &layer, const engine::tilemap::TileMap &map)
    {
        const auto entity = _registry.create();

        _registry.emplace<components::Transform>(entity, glm::vec2(0.0, 0.0));
        _registry.emplace<components::Ordering>(entity, layer.z_index());
        _registry.emplace<components::Renderable2D>(entity);

        const auto &tiles = layer.tiles();
        const auto &dimensions = layer.dimensions();

        std::vector<components::TileLayer::TileData> tile_data(dimensions.x * dimensions.y);
        for (int x = 0; x < dimensions.x; ++x)
        {
            for (int y = 0; y < dimensions.y; ++y)
            {
                int index = layer.index(x, y);
                const auto gid = tiles[index];
                const auto tile = map.get(gid);

                if (tile)
                {
                    const auto tileset = tile->tileset;
                    if (tileset)
                    {
                        const engine::Texture2D &tileset_texture = tileset->texture();
                        const engine::Texture2D &luminosity_texture = tileset->luminosity_texture();

                        const engine::Rectangle tile_dest_rect(
                            glm::vec2(x * tileset->tile_dimensions().x, y * tileset->tile_dimensions().y),
                            tileset->tile_dimensions());
                        const auto tile_source_rect = tileset->source_rect(tile->id);

                        components::TileLayer::TileData data(engine::Texture2DContainer<2>{&tileset_texture, &luminosity_texture}, tile_dest_rect, tile_source_rect);
                        if (tile->collision_shape)
                        {
                            data.collision_shape = tile->collision_shape.get();
                        }

                        tile_data[index] = data;
                    }
                }
                else
                {
                    components::TileLayer::TileData data;
                    tile_data[index] = data;
                }
            }
        }
        _registry.emplace<components::TileLayer>(entity, std::ref(layer), tile_data);

        return entity;
    }

    const entt::entity Map::create_tileset_entity(const engine::tilemap::Tileset &tileset)
    {
        const auto entity = _registry.create();
        _registry.emplace<components::Tileset>(entity, std::ref(tileset));

        return entity;
    }

    const entt::entity Map::create_sprite_entity(const engine::tilemap::TileObject &object, const engine::tilemap::TileMap &map)
    {
        const auto &data = object.data;

        const glm::vec2 sprite_position(data.getPosition().x, data.getPosition().y);

        int tile_gid = data.getGid();
        const engine::tilemap::Tile *tile = map.get(tile_gid);
        if (!tile)
        {
            throw "Tile not found";
        }

        const engine::tilemap::Tileset *tileset_of_sprite = tile->tileset;
        if (!tileset_of_sprite)
        {
            throw "corresponding tileset not found";
        }
        const engine::Texture2D &texture = tileset_of_sprite->texture();
        const engine::Texture2D &luminosity_texture = tileset_of_sprite->luminosity_texture();

        const auto &sprite_class_name = tile->class_type;
        const auto &sprite_properties = tile->properties;
        // 'const_cast' is okay, because tson::PropertyCollection::get<> should have been declared 'const'
        const auto &sprite_state_name = const_cast<tson::PropertyCollection &>(sprite_properties).getValue<std::string>(Map::FIELD_STATE);
        const auto &sprite_state = (*tileset_of_sprite)[sprite_class_name][sprite_state_name];

        const auto entity = _registry.create();
        _registry.emplace<components::Transform>(entity, sprite_position);
        _registry.emplace<components::Ordering>(entity, 3.0);
        _registry.emplace<components::Renderable2D>(entity);
        const auto &animation_component = _registry.emplace<components::Animation>(entity, 0.0, 0, sprite_state.frames);
        _registry.emplace<components::Sprite>(entity, engine::Texture2DContainer<2>{&texture, &luminosity_texture}, animation_component.get_current_frame().source_rect);

        if (tile->collision_shape)
        {
            _registry.emplace<components::Collider>(entity, std::unique_ptr<engine::Shape>(tile->collision_shape->clone()));
        }

        // 'const_cast' is okay, because tson::Object::getProperties should have been declared 'const'
        const auto &properties = const_cast<tson::Object &>(data).getProperties();
        // 'const_cast' is okay, because tson::PropertyCollection::getProperty should have been declared 'const'
        const auto *script_path_property = const_cast<tson::PropertyCollection &>(properties).getProperty(Map::FIELD_SCRIPT);
        if (script_path_property)
        {
            std::filesystem::path script_path = map.resource_path().parent_path() / script_path_property->getValue<std::string>();
            _registry.emplace<components::ScriptLoader>(entity, script_path);
        }

        return entity;
    }
}