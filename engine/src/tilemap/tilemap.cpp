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
                sprite_sheet.resource_path(tileset_path);

                // const_cast is okay, because the original tileset vector was not const
                sprite_sheet.parse(const_cast<tson::Tileset &>(tson_tileset), resource_manager);

                engine::sprite::SpriteSheet &sprite_sheet_resource = resource_manager.emplace_resource<engine::sprite::SpriteSheet>(tileset_name, sprite_sheet);

                std::unique_ptr<Tileset> tileset = std::make_unique<Tileset>(sprite_sheet_resource, tson_tileset);
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
                    tile_layer->data(tile_data);

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
