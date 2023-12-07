#pragma once

#include <vector>

#include "tileson/tileson.hpp"

#include "core/texture2d.hpp"
#include "core/rectangle.hpp"
#include "graphics/spritesheet.hpp"

namespace engine::tilemap
{
    class Tileset
    {
    protected:
        engine::graphics::SpriteSheet &_sprite_sheet;
        tson::Tileset _native_tileset;

    public:
        Tileset(engine::graphics::SpriteSheet &sprite_sheet, const tson::Tileset &native_tileset)
            : _sprite_sheet(sprite_sheet), _native_tileset(native_tileset)
        {
        }

        bool has_tile(unsigned int id) const;
        engine::Rectangle source_rect(unsigned int id) const;

        Texture2D &texture() { return _sprite_sheet.texture(); }
        const Texture2D &texture() const { return _sprite_sheet.texture(); }

        int tile_width() const { return _sprite_sheet.tile_width(); }
        int tile_height() const { return _sprite_sheet.tile_height(); }

        int first_GID() const { return _native_tileset.getFirstgid(); }
        int last_GID() const { return _native_tileset.getFirstgid() + _native_tileset.getTileCount() - 1; }

        std::string name() const { return _sprite_sheet.resource_name(); }

        tson::Tile *tile(int gid);
    };
}