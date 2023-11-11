#include "tilemap/tilemap.hpp"

#include <vector>

#include "tileson/tileson.hpp"

#include "core/log.hpp"
#include "core/resourcemanager.hpp"
#include "core/texture2d.hpp"

#include "tilemap/tileobject.hpp"

namespace engine::tilemap
{
    bool TileMap::load_resource(ResourceManager &resource_manager, va_list args)
    {
        tson::Tileson t(std::make_unique<tson::Json11>());
        std::unique_ptr<tson::Map> tson_map = t.parse(_resource_path);

        if (tson_map->getStatus() == tson::ParseStatus::OK)
        {
            const auto map_size = tson_map->getSize();

            const auto &tson_tilesets = tson_map->getTilesets();
            for (const auto &tson_tileset : tson_tilesets)
            {
                const std::string tileset_name = tson_tileset.getName();

                // TODO Tileson does not provide access to the path, maybe backward engineer it
                const std::string tileset_path = "";

                engine::sprite::SpriteSheet sprite_sheet;
                sprite_sheet.set_resource_path(tileset_path);
                sprite_sheet.parse(tson_tileset, resource_manager);

                engine::sprite::SpriteSheet &sprite_sheet_resource = resource_manager.emplace_resource<engine::sprite::SpriteSheet>(tileset_name, sprite_sheet);

                auto first_gid = tson_tileset.getFirstgid();
                auto last_gid = first_gid + tson_tileset.getTileCount() - 1;

                std::unique_ptr<Tileset> tileset = std::make_unique<Tileset>(sprite_sheet_resource, first_gid, last_gid);
                _tilesets.push_back(std::move(tileset));
            }

            auto &tson_layers = tson_map->getLayers();
            int z_index = 0;
            for (auto &tson_layer : tson_layers)
            {
                if (tson_layer.getType() == tson::LayerType::ObjectGroup)
                {
                    const auto &tson_objects = tson_layer.getObjects();
                    for (const auto &tson_object : tson_objects)
                    {
                        int tile_gid = tson_object.getGid();
                        const tson::Tileset *tson_tileset = nullptr;
                        for (const auto &tileset_local : tson_tilesets)
                        {
                            auto first_gid = tileset_local.getFirstgid();
                            auto last_gid = first_gid + tileset_local.getTileCount() - 1;

                            if (tile_gid >= first_gid && tile_gid <= last_gid)
                            {
                                tson_tileset = &tileset_local;
                                break;
                            }
                        }

                        if (tson_tileset)
                        {
                            // We have to interpret the object as a sprite and the tileset as its sprite sheet
                            // Need to inherit class from tileset

                            const std::string tileset_name = tson_tileset->getName();

                            // We now that the sprite sheet is a resource, as it was loaded before
                            const engine::sprite::SpriteSheet &sprite_sheet = resource_manager.get<engine::sprite::SpriteSheet>(tileset_name);

                            // Some ID magic
                            const int tile_id = tile_gid - tson_tileset->getFirstgid() + 1;
                            const tson::Tile *tson_tile = const_cast<tson::Tileset *>(tson_tileset)->getTile(tile_id);
                        }
                        else
                        {
                            // TODO
                        }
                    }
                }
                else if (tson_layer.getType() == tson::LayerType::TileLayer)
                {
                    std::vector<Tile> tile_data(map_size.x * map_size.y);
                    for (int x = 0; x < map_size.x; ++x)
                    {
                        for (int y = 0; y < map_size.y; ++y)
                        {
                            const tson::Tile *tson_tile = tson_layer.getTileData(x, y);

                            int id = 0;
                            if (tson_tile)
                            {
                                id = tson_tile->getGid();
                            }
                            tile_data[x + map_size.x * y].ID = id;
                        }
                    }

                    std::unique_ptr<TileLayer> tile_layer = std::make_unique<TileLayer>(map_size.x, map_size.y, z_index);
                    tile_layer->set_data(tile_data);

                    _layers.push_back(std::move(tile_layer));
                    z_index++;
                }
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
