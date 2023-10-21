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
    };
}