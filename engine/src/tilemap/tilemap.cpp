#include "tilemap/tilemap.hpp"

#include <vector>

#include "nlohmann/json.hpp"
#include "tileson/tileson.hpp"

#include "core/log.hpp"
#include "core/resourcemanager.hpp"
#include "core/texture2d.hpp"

/*#include "tilemap/tileobject.hpp"

#include "sprite/spritesheet.hpp"*/

namespace engine::tilemap
{
    bool TileMap::load_resource(ResourceManager &resource_manager, va_list args)
    {
        tson::Tileson t(std::make_unique<tson::NlohmannJson>());
        std::unique_ptr<tson::Map> map = t.parse(_resource_path);

        std::filesystem::path working_dir = _resource_path.parent_path();

        if (map->getStatus() == tson::ParseStatus::OK)
        {
            const auto map_size = map->getSize();

            const auto &tilesets = map->getTilesets();
            for (const auto &tileset : tilesets)
            {
                const auto tileset_name = tileset.getName();
                const auto tileset_image_path = working_dir / tileset.getImagePath();
                auto first_gid = tileset.getFirstgid();
                auto last_gid = first_gid + tileset.getTileCount();
                auto tileset_texture = resource_manager.load_resource<Texture2D>(tileset_name, tileset_image_path);
                auto tile_width = tileset.getTileSize().x;
                auto tile_height = tileset.getTileSize().y;

                Tileset tileset_object(*tileset_texture, first_gid, last_gid, tile_width, tile_height);
                Tileset &tileset_resource = resource_manager.emplace_resource<Tileset>(tileset_name + "_tileset", tileset_object);

                _tilesets.push_back(&tileset_resource);
            }

            auto &res_layers = map->getLayers();
            int z_index = 0;
            for (auto &res_layer : res_layers)
            {
                if (res_layer.getType() == tson::LayerType::ObjectGroup)
                {
                    /*const auto &objectLayer = res_layer->getLayerAs<tmx::ObjectGroup>();
                    const auto &objects = objectLayer.getObjects();
                    for (const auto &object : objects)
                    {
                        int tile_id = object.getTileID();
                        const Tileset *tileset = nullptr;
                        for (const auto &t : _tilesets)
                        {
                            if (t->has_tile(tile_id))
                            {
                                tileset = t;
                                break;
                            }
                        }

                        if (tileset)
                        {
                            // We have to interpret the object as a sprite and the tileset as its sprite sheet
                        }
                        else
                        {
                            // TODO
                        }
                    }*/
                }
                else if (res_layer.getType() == tson::LayerType::TileLayer)
                {
                    std::vector<Tile> tile_data(map_size.x * map_size.y);
                    for (int x = 0; x < map_size.x; ++x)
                    {
                        for (int y = 0; y < map_size.y; ++y)
                        {
                            const tson::Tile *res_tile_data = res_layer.getTileData(x, y);

                            int id = 0;
                            if (res_tile_data)
                            {
                                id = res_tile_data->getId();
                            }
                            tile_data[x + map_size.x * y].ID = id;
                        }
                    }

                    TileLayer tile_layer(map_size.x, map_size.y, z_index);
                    tile_layer.set_data(tile_data);

                    _layers.push_back(std::make_unique<TileLayer>(tile_layer));
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
