#pragma once

#include <vector>
#include <memory>

#include "tileson/tileson.hpp"

#include "core/texture2d.hpp"
#include "core/shape.hpp"
#include "core/rectangle.hpp"
#include "core/resource.hpp"
#include "graphics/spritesheet.hpp"
#include "tilemap/tile.hpp"

namespace engine::tilemap
{
    class Tileset : public Resource, public engine::graphics::SpriteSheet
    {
    private:
        inline static const std::string NAME_COLLISION_SHAPE = "collision";

        std::vector<Tile> _tiles;
        std::unique_ptr<const Shape> parse_shape(const tson::Object &object) const;

    public:
        Tileset() {}

        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        void parse(const tson::Tileset &tson_tileset, ResourceManager &resource_manager);
        const Tile *get(int id) const;
    };
}