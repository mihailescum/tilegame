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

namespace tilegame::systems
{
    MapSystem::MapSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void MapSystem::load_content()
    {
        auto &resource_manager = _scene.game().resource_manager();
        const engine::tilemap::TileMap &map = *resource_manager.load_resource<engine::tilemap::TileMap>("map1", "content/maps/map1.tmj");
        create_map_entity(map);
    }

    const entt::entity MapSystem::create_map_entity(const engine::tilemap::TileMap &map)
    {
        const auto entity = _registry.create();
        _registry.emplace<components::TileMap>(entity, map);
        _registry.emplace<components::Transform>(entity, glm::vec2(0.0), glm::vec2(0.0));

        const tilegame::SceneGraphData map_scenedata(entity);
        tilegame::SceneGraphNode &map_scenenode = _scene.scene_graph_root().add_child(map_scenedata);
        _registry.emplace<components::SceneNode>(entity, &map_scenenode);

        std::vector<entt::entity> tileset_entities;
        for (const auto &tileset : map.tilesets())
        {
            const auto tileset_entity = create_tileset_entity(*tileset);
            tileset_entities.push_back(tileset_entity);
        }

        for (const auto &layer : map.layers())
        {
            const auto layer_entity = create_layer_entity(*layer, map);

            const tilegame::SceneGraphData layer_scenedata(layer_entity);
            tilegame::SceneGraphNode &layer_scenenode = map_scenenode.add_child(layer_scenedata);
            _registry.emplace<components::SceneNode>(layer_entity, &layer_scenenode);
        }

        for (const auto &object : map.objects())
        {
            const auto &data = object->data;
            if (data.getGid() > 0) // parse a sprite
            {
                const auto sprite_entity = create_sprite_entity(*object, map);
                const tilegame::SceneGraphData sprite_scenedata(sprite_entity);
                tilegame::SceneGraphNode &sprite_scenenode = map_scenenode.add_child(sprite_scenedata);
                _registry.emplace<components::SceneNode>(sprite_entity, &sprite_scenenode);
            }
            else
            {
                // TODO
            }
        }

        return entity;
    }

    const entt::entity MapSystem::create_layer_entity(const engine::tilemap::TileLayer &layer, const engine::tilemap::TileMap &map)
    {
        const auto entity = _registry.create();

        _registry.emplace<components::Transform>(entity, glm::vec2(0.0, 0.0), glm::vec2(0.0));
        _registry.emplace<components::Ordering>(entity, layer.z_index());
        _registry.emplace<components::Renderable2D>(entity);

        const auto tiles = layer.tiles();
        const auto width = layer.width();
        const auto height = layer.height();

        std::vector<components::TileLayer::TileData> tile_data;
        tile_data.reserve(width * height);
        for (int x = 0; x < width; ++x)
        {
            for (int y = 0; y < height; ++y)
            {
                const auto &tile = tiles[x + width * y];

                const engine::tilemap::Tileset *tileset_containing_tile = nullptr;
                for (const auto &tileset : map.tilesets())
                {
                    if (tileset->has_tile(tile.ID))
                    {
                        tileset_containing_tile = tileset.get();
                        break;
                    }
                }

                if (tileset_containing_tile)
                {
                    const engine::Texture2D &tileset_texture = tileset_containing_tile->texture();
                    const auto tile_width = tileset_containing_tile->tile_width();
                    const auto tile_height = tileset_containing_tile->tile_height();

                    const engine::Rectangle tile_dest_rect(x * tile_width, y * tile_height, tile_width, tile_height);
                    const auto tile_source_rect = tileset_containing_tile->source_rect(tile.ID);

                    components::TileLayer::TileData data{tileset_texture, tile_dest_rect, tile_source_rect};
                    tile_data.push_back(data);
                }
            }
        }
        _registry.emplace<components::TileLayer>(entity, layer, tile_data);

        return entity;
    }

    const entt::entity MapSystem::create_tileset_entity(const engine::tilemap::Tileset &tileset)
    {
        const auto entity = _registry.create();
        _registry.emplace<components::Tileset>(entity, tileset);

        return entity;
    }

    const entt::entity MapSystem::create_sprite_entity(const engine::tilemap::TileObject &object, const engine::tilemap::TileMap &map)
    {
        const auto &data = object.data;

        const glm::vec2 sprite_position(data.getPosition().x, data.getPosition().y);

        int tile_gid = data.getGid();
        engine::tilemap::Tileset *tileset_of_sprite = const_cast<engine::tilemap::TileMap &>(map).find_tileset_by_gid(tile_gid);

        if (!tileset_of_sprite)
        {
            throw "corresponding tileset not found";
        }

        // We have to interpret the object as a sprite and the tileset as its sprite sheet
        // Need to inherit class from tileset

        const std::string tileset_name = tileset_of_sprite->name();

        // We know that the sprite sheet is a resource, as it was loaded before
        auto &resource_manager = _scene.game().resource_manager();
        const engine::sprite::SpriteSheet &sprite_sheet = resource_manager.get<engine::sprite::SpriteSheet>(tileset_name);
        const engine::Texture2D &texture = sprite_sheet.texture();

        auto *tile = tileset_of_sprite->tile(tile_gid);
        const auto &sprite_class_name = tile->getClassType();
        const auto &sprite_state_name = tile->get<std::string>(MapSystem::FIELD_STATE);
        const auto &sprite_state = sprite_sheet[sprite_class_name][sprite_state_name];

        const auto entitiy = _registry.create();
        _registry.emplace<components::Transform>(entitiy, sprite_position, glm::vec2(0.0));
        _registry.emplace<components::Ordering>(entitiy, 3.0);
        //_registry.emplace<components::Movement>(entitiy, components::Movement::None, 100.0);
        _registry.emplace<components::Renderable2D>(entitiy);
        const auto &animation_component = _registry.emplace<components::Animation>(entitiy, 0.0, 0, sprite_state.frames);
        _registry.emplace<components::Sprite>(entitiy, texture, animation_component.get_current_frame().source_rect);

        auto &properties = const_cast<tson::Object &>(data).getProperties();
        const auto *script_path_property = properties.getProperty(MapSystem::FIELD_SCRIPT);
        if (script_path_property)
        {
            std::filesystem::path script_path = map.resource_path().parent_path() / script_path_property->getValue<std::string>();
            _registry.emplace<components::ScriptLoader>(entitiy, script_path);
        }

        return entitiy;
    }
}