#pragma once

#include <vector>
#include <memory>

#include "tileson/tileson.hpp"

#include "core/resource.hpp"
#include "tilemap/tilelayer.hpp"
#include "tilemap/tileset.hpp"
#include "tilemap/tileobject.hpp"
#include "tilemap/tile.hpp"

namespace engine::tilemap
{
    class TileMap : public engine::Resource
    {
    private:
        int _width;
        int _height;

        std::vector<std::unique_ptr<TileLayer>> _layers;
        std::vector<std::tuple<int, int, std::unique_ptr<Tileset>>> _tilesets;
        std::vector<std::unique_ptr<TileObject>> _objects;

        void parse_tilesets(const tson::Map &tson_map, ResourceManager &resource_manager);
        void parse_layers(const tson::Map &tson_map, ResourceManager &resource_manager);
        void parse_objectgroup(const tson::Layer &tson_layer, ResourceManager &resource_manager);
        void parse_tilelayer(const tson::Layer &tson_layer, ResourceManager &resource_manager, int z_index);

        int get_gid_at(const tson::Layer &tson_layer, int x, int y) const;
        const Tileset *get_tileset_from_gid(int gid, int &id) const;

    public:
        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        const Tile *get(int gid) const;

        // TODO return iterators instead
        const std::vector<std::unique_ptr<TileLayer>> &layers() const { return _layers; }
        const std::vector<std::unique_ptr<TileObject>> &objects() const { return _objects; }
        const auto &tilesets() const { return _tilesets; }
    };
}