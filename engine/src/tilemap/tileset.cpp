#include "tilemap/tileset.hpp"

namespace engine::tilemap
{
    bool Tileset::has_tile(unsigned int id) const
    {
        return id >= _first_GID && id <= _last_GID;
    }

    engine::Rectangle Tileset::get_source_rect(unsigned int id) const
    {
        id -= _first_GID;

        int tile_width = _sprite_sheet.get_tile_width();
        int tile_height = _sprite_sheet.get_tile_height();
        int tex_width = _sprite_sheet.get_texture().get_width();
        int x = (id % (tex_width / tile_width)) * tile_width;
        int y = (id / (tex_width / tile_width)) * tile_width;

        engine::Rectangle result(x, y, tile_width, tile_height);
        return result;
    }

} // namespace engine::tilemap
