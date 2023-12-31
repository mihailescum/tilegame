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
    class Tileset : public Resource
    {
    private:
        inline static const std::string NAME_COLLISION_SHAPE = "collision";

        const engine::graphics::SpriteSheet *_sprite_sheet;
        std::vector<Tile> _tiles;
        std::unique_ptr<const Shape> parse_shape(const tson::Object &object) const;

    public:
        Tileset() {}
        Tileset(const engine::graphics::SpriteSheet &sprite_sheet) : _sprite_sheet(&sprite_sheet) {}

        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        void parse(const tson::Tileset &tson_tileset, ResourceManager &resource_manager);
        const Tile *get(int id) const;

        engine::Rectangle source_rect(int id) const;

        const engine::graphics::SpriteSheet *sprite_sheet() const { return _sprite_sheet; }

        // std::string name() const { return _sprite_sheet.resource_name(); }
    };
}