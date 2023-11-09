#pragma once

#include <vector>

#include "core/texture2d.hpp"
#include "core/rectangle.hpp"
#include "sprite/spritesheet.hpp"

namespace engine::tilemap
{
    class Tileset
    {
    protected:
        unsigned int _first_GID;
        unsigned int _last_GID;
        engine::sprite::SpriteSheet &_sprite_sheet;

    public:
        Tileset(engine::sprite::SpriteSheet &sprite_sheet, unsigned int first_GID, unsigned int last_GID)
            : _sprite_sheet(sprite_sheet), _first_GID(first_GID), _last_GID(last_GID) {}

        bool has_tile(unsigned int id) const;
        engine::Rectangle get_source_rect(unsigned int id) const;

        Texture2D &get_texture() { return _sprite_sheet.get_texture(); }
        const Texture2D &get_texture() const { return _sprite_sheet.get_texture(); }

        int get_tile_width() const { return _sprite_sheet.get_tile_width(); }
        int get_tile_height() const { return _sprite_sheet.get_tile_height(); }
    };
}