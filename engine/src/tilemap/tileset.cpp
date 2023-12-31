#include "tilemap/tileset.hpp"

namespace engine::tilemap
{
    bool Tileset::has_tile(unsigned int id) const
    {
        return id >= first_GID() && id <= last_GID();
    }

    engine::Rectangle Tileset::source_rect(unsigned int id) const
    {
        id -= first_GID();

        int tile_width = _sprite_sheet.tile_width();
        int tile_height = _sprite_sheet.tile_height();
        int tex_width = _sprite_sheet.texture().width();
        int x = (id % (tex_width / tile_width)) * tile_width;
        int y = (id / (tex_width / tile_width)) * tile_width;

        engine::Rectangle result(x, y, tile_width, tile_height);
        return result;
    }

    const tson::Tile *Tileset::tile(int gid) const
    {
        if (has_tile(gid))
        {
            // Some ID magic
            const int tile_id = gid - first_GID() + 1;
            // 'const_cast' is okay, because tson::Tileset::getTile should have been declared 'const'
            const tson::Tile *tson_tile = const_cast<tson::Tileset &>(_native_tileset).getTile(tile_id);
            return tson_tile;
        }
        else
        {
            return nullptr;
        }
    }
} //
