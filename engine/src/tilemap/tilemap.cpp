#include "tilemap/tilemap.hpp"

#include <vector>
#include <filesystem>

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
        // const tson::Map &tson_map = *(t.parse(_resource_path));
        const auto tson_map_ptr = t.parse(_resource_path);
        const auto &tson_map = *tson_map_ptr;

        if (tson_map.getStatus() == tson::ParseStatus::OK)
        {
            const auto map_size = tson_map.getSize();

            // 'const_cast' is okay, because tson::Map::getTilesets should have been declared 'const'
            const auto &tson_tilesets = const_cast<tson::Map &>(tson_map).getTilesets();
            for (const auto &tson_tileset : tson_tilesets)
            {
                const std::string tileset_name = tson_tileset.getName();

                // TODO Tileson does not provide access to the path, maybe backward engineer it
                const std::filesystem::path tileset_path = "";

                engine::graphics::SpriteSheet sprite_sheet;
                sprite_sheet.resource_path(tileset_path);

                sprite_sheet.parse(tson_tileset, resource_manager);

                engine::graphics::SpriteSheet &sprite_sheet_resource = resource_manager.emplace_resource<engine::graphics::SpriteSheet>(tileset_name, sprite_sheet);

                std::unique_ptr<Tileset> tileset = std::make_unique<Tileset>(sprite_sheet_resource, tson_tileset);
                _tilesets.push_back(std::move(tileset));
            }

            // 'const_cast' is okay, because tson::Map::getLayers should have been declared 'const'
            const auto &tson_layers = const_cast<tson::Map &>(tson_map).getLayers();
            int z_index = 0;
            for (auto &tson_layer : tson_layers)
            {
                if (tson_layer.getType() == tson::LayerType::ObjectGroup)
                {
                    // 'const_cast' is okay, because tson::Layer::getObjects should have been declared 'const'
                    const auto &tson_objects = const_cast<tson::Layer &>(tson_layer).getObjects();
                    for (const auto &tson_object : tson_objects)
                    {
                        std::unique_ptr<TileObject> object = std::make_unique<TileObject>(tson_object);
                        _objects.push_back(std::move(object));
                    }
                }
                else if (tson_layer.getType() == tson::LayerType::TileLayer)
                {
                    std::vector<Tile> tile_data(map_size.x * map_size.y);
                    for (int x = 0; x < map_size.x; ++x)
                    {
                        for (int y = 0; y < map_size.y; ++y)
                        {
                            // 'const_cast' is okay, because tson::Layer::getTileData should have been declared 'const'
                            const tson::Tile *tson_tile = const_cast<tson::Layer &>(tson_layer).getTileData(x, y);

                            int id = 0;
                            if (tson_tile)
                            {
                                id = tson_tile->getGid();
                            }
                            tile_data[x + map_size.x * y].ID = id;
                        }
                    }

                    std::unique_ptr<TileLayer> tile_layer = std::make_unique<TileLayer>(map_size.x, map_size.y, z_index);
                    tile_layer->data(tile_data);

                    _layers.push_back(std::move(tile_layer));
                }
                z_index++;
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

    const Tileset *TileMap::find_tileset_by_gid(int gid) const
    {
        for (const auto &tileset : _tilesets)
        {
            auto first_gid = tileset->first_GID();
            auto last_gid = tileset->last_GID();

            if (gid >= first_gid && gid <= last_gid)
            {
                return tileset.get();
            }
        }
        return nullptr;
    }

    Tileset *TileMap::find_tileset_by_gid(int gid)
    {
        return const_cast<Tileset *>(const_cast<const TileMap *>(this)->find_tileset_by_gid(gid));
    }
} // namespace engine::tilemap
