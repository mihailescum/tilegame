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
            const auto map_dimensions = tson_map.getSize();
            _dimensions.x = map_dimensions.x;
            _dimensions.y = map_dimensions.y;

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

            // TODO Tileson does not provide access to the path, we backward engineer it
            std::string image_path = tson_tileset.getFullImagePath();
            const std::string image_name = tson_tileset.getImagePath();
            image_path.erase(image_path.rfind(image_name), image_name.size());

            const std::filesystem::path tileset_path = std::filesystem::path(image_path) / (tileset_name + ".tsj");

            std::unique_ptr<Tileset> tileset = std::make_unique<Tileset>();
            tileset->resource_name(tileset_name);
            tileset->resource_path(tileset_path);
            tileset->parse(tson_tileset, resource_manager);
            const Tileset &tileset_resource = resource_manager.emplace_resource<Tileset>(tileset_name, tileset);

            const int first_gid = tson_tileset.getFirstgid();
            const int last_gid = first_gid + tson_tileset.getTileCount() - 1;
            _tilesets.push_back({first_gid, last_gid, &tileset_resource});
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
                parse_objectgroup(tson_layer, resource_manager);
            }
            else if (tson_layer.getType() == tson::LayerType::TileLayer)
            {
                parse_tilelayer(tson_layer, resource_manager, z_index);
            }
            z_index++;
        }
    }

    void TileMap::parse_objectgroup(const tson::Layer &tson_layer, ResourceManager &resource_manager)
    {
        // 'const_cast' is okay, because tson::Layer::getObjects should have been declared 'const'
        const auto &tson_objects = const_cast<tson::Layer &>(tson_layer).getObjects();
        for (const auto &tson_object : tson_objects)
        {
            std::unique_ptr<TileObject> object = std::make_unique<TileObject>(tson_object);
            _objects.push_back(std::move(object));
        }
    }

    void TileMap::parse_tilelayer(const tson::Layer &tson_layer, ResourceManager &resource_manager, int z_index)
    {
        const auto &tile_dimensions = tson_layer.getMap()->getTileSize();

        std::unique_ptr<TileLayer> tilelayer = std::make_unique<TileLayer>(_dimensions, glm::ivec2(tile_dimensions.x, tile_dimensions.y), z_index);
        std::vector<int> tile_data(tilelayer->dimensions().x * tilelayer->dimensions().y);
        for (int x = 0; x < tilelayer->dimensions().x; ++x)
        {
            for (int y = 0; y < tilelayer->dimensions().y; ++y)
            {
                tile_data[tilelayer->index(x, y)] = get_gid_at(tson_layer, x, y);
            }
        }
        tilelayer->data(tile_data);

        _layers.push_back(std::move(tilelayer));
    }

    int TileMap::get_gid_at(const tson::Layer &tson_layer, int x, int y) const
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

    const Tile *TileMap::get(int gid) const
    {
        int id;
        const auto tileset = get_tileset_from_gid(gid, id);
        if (tileset)
        {
            return tileset->get(id);
        }
        else
        {
            return nullptr;
        }
    }

    const Tileset *TileMap::get_tileset_from_gid(int gid, int &id) const
    {
        for (const auto &[first_gid, last_gid, tileset] : _tilesets)
        {
            if (gid >= first_gid && gid <= last_gid)
            {
                id = gid - first_gid;
                return tileset;
            }
        }
        return nullptr;
    }

    void TileMap::unload_resource()
    {
    }
} // namespace engine::tilemap
