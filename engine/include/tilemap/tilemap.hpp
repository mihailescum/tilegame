#pragma once

#include <vector>

#include <tmxlite/Map.hpp>

#include "core/resource.hpp"
#include "tilemap/tilelayer.hpp"
#include "tilemap/tileset.hpp"

namespace engine::tilemap
{
    class TileMap : public engine::Resource
    {
    private:
        std::vector<TileLayer> _layers;
        std::vector<Tileset> _tilesets;

    public:
        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        const std::vector<TileLayer> &get_layers() const { return _layers; }
        const std::vector<Tileset> &get_tilesets() const { return _tilesets; }
    };
}