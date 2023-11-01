#include "tilemap/tilemap.hpp"

#include <vector>

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include "core/log.hpp"
#include "core/resourcemanager.hpp"
#include "core/texture2d.hpp"

namespace engine::tilemap
{
    bool TileMap::load_resource(ResourceManager &resource_manager, va_list args)
    {
        // From the documentation: "tmx::map is not made to be kept around"!
        tmx::Map map;
        if (map.load(_resource_path))
        {
            const auto map_size = map.getTileCount();

            const auto &res_layers = map.getLayers();
            int z_index = 0;
            for (const auto &res_layer : res_layers)
            {
                if (res_layer->getType() == tmx::Layer::Type::Object)
                {
                    const auto &objectLayer = res_layer->getLayerAs<tmx::ObjectGroup>();
                    const auto &objects = objectLayer.getObjects();
                    for (const auto &object : objects)
                    {
                        // do stuff with object properties
                    }
                }
                else if (res_layer->getType() == tmx::Layer::Type::Tile)
                {
                    const auto &res_tile_layer = res_layer->getLayerAs<tmx::TileLayer>();
                    const auto &res_tiles = res_tile_layer.getTiles();

                    TileLayer tile_layer(map_size.x, map_size.y, z_index);
                    tile_layer.set_data(res_tiles);

                    _layers.push_back(tile_layer);
                    z_index++;
                }
            }

            const auto &tilesets = map.getTilesets();
            for (const auto &tileset : tilesets)
            {
                const auto tileset_name = tileset.getName();
                const auto tileset_image_path = tileset.getImagePath();
                auto first_gid = tileset.getFirstGID();
                auto last_gid = tileset.getLastGID();
                auto tileset_texture = resource_manager.load_resource<Texture2D>(tileset_name, tileset_image_path);
                auto tile_width = tileset.getTileSize().x;
                auto tile_height = tileset.getTileSize().y;

                _tilesets.push_back(Tileset(*tileset_texture, first_gid, last_gid, tile_width, tile_height));
            }
            return true;
        }
        else
        {
            Log::e("Failed to load map. FILE: ", _resource_path);
            return false;
        }
    }

    void TileMap::unload_resource()
    {
    }
} // namespace engine::tilemap
