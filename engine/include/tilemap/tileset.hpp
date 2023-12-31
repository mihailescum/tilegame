#pragma once

#include <vector>
#include <memory>

#include "tileson/tileson.hpp"

#include "core/texture2d.hpp"
#include "core/shape.hpp"
#include "core/rectangle.hpp"
#include "graphics/spritesheet.hpp"
#include "tilemap/tile.hpp"

namespace engine::tilemap
{
    class Tileset
    {
    private:
        inline static const std::string NAME_COLLISION_SHAPE = "collision";

        engine::graphics::SpriteSheet &_sprite_sheet;
        std::vector<Tile> _tiles;
        std::unique_ptr<const Shape> parse_shape(const tson::Object &object) const;

    public:
        Tileset(engine::graphics::SpriteSheet &sprite_sheet) : _sprite_sheet(sprite_sheet) {}

        void parse(const tson::Tileset &tson_tileset);
        const Tile *get(int id) const;

        engine::Rectangle source_rect(int id) const;

        Texture2D &texture() { return _sprite_sheet.texture(); }
        const Texture2D &texture() const { return _sprite_sheet.texture(); }

        int tile_width() const { return _sprite_sheet.tile_width(); }
        int tile_height() const { return _sprite_sheet.tile_height(); }

        std::string name() const { return _sprite_sheet.resource_name(); }
    };
}