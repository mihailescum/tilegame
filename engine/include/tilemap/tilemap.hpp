#pragma once

#include <vector>
#include <memory>

#include "core/resource.hpp"
#include "tilemap/tilelayer.hpp"
#include "tilemap/tileset.hpp"
#include "tilemap/tileobject.hpp"

namespace engine::tilemap
{
    class TileMap : public engine::Resource
    {
    private:
        std::vector<std::unique_ptr<TileLayer>> _layers;
        std::vector<std::unique_ptr<Tileset>> _tilesets;
        std::vector<std::unique_ptr<TileObject>> _objects;

    public:
        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        const std::vector<std::unique_ptr<TileLayer>> &get_layers() const { return _layers; }
        const std::vector<std::unique_ptr<Tileset>> &get_tilesets() const { return _tilesets; }
        const std::vector<std::unique_ptr<TileObject>> &get_objects() const { return _objects; }
    };
}