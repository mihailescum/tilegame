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
            _width = map_size.x;
            _height = map_size.y;

            parse_tilesets(tson_map, resource_manager);
            parse_layers(tson_map, resource_manager);

            return true;
        }
        else
        {
            Log::e("Failed to load map. FILE: ", _resource_path);
            return false;
        }
    }

    void TileMap::parse_tilesets(const tson::Map &tson_map, ResourceManager &resource_manager)
    {
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
    }

    void TileMap::parse_layers(const tson::Map &tson_map, ResourceManager &resource_manager)
    {
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
                std::vector<Tile> tile_data(_width * _height);
                for (int x = 0; x < _width; ++x)
                {
                    for (int y = 0; y < _height; ++y)
                    {
                        auto &tile = tile_data[x + _width * y];

                        int gid = get_ID_at(tson_layer, x, y);
                        const Tileset *tileset = get_tileset_from_gid(gid);
                        if (tileset)
                        {
                            tile.width = tileset->tile_width();
                            tile.height = tileset->tile_height();
                            tile.tileset = tileset;
                        }

                        tile.gid = gid;
                    }
                }

                std::unique_ptr<TileLayer> tile_layer = std::make_unique<TileLayer>(_width, _height, z_index);
                tile_layer->data(tile_data);

                _layers.push_back(std::move(tile_layer));
            }
            z_index++;
        }
    }

    int TileMap::get_ID_at(const tson::Layer &tson_layer, int x, int y)
    {
        // 'const_cast' is okay, because tson::Layer::getTileData should have been declared 'const'
        const tson::Tile *tson_tile = const_cast<tson::Layer &>(tson_layer).getTileData(x, y);

        if (tson_tile)
        {
            return tson_tile->getGid();
        }
        else
        {
            return 0;
        }
    }

    const Tileset *TileMap::get_tileset_from_gid(int gid)
    {
        for (const auto &tileset : _tilesets)
        {
            if (tileset->has_tile(gid))
            {
                return tileset.get();
            }
        }
        return nullptr;
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
