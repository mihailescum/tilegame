#include "tilemap/tileset.hpp"

namespace engine::tilemap
{
    Tileset::Tileset(engine::Texture2D &texture, unsigned int first_GID, unsigned int last_GID, int tile_width, int tile_height)
        : _texture(texture),
          _first_GID(first_GID),
          _last_GID(last_GID),
          _tile_width(tile_width),
          _tile_height(tile_height)
    {
    }

    bool Tileset::has_tile(unsigned int id) const
    {
        return id >= _first_GID && id <= _last_GID;
    }

    engine::Rectangle Tileset::get_source_rect(unsigned int id) const
    {
        id -= _first_GID;
        int x = (id % (_texture.get_width() / _tile_width)) * _tile_width;
        int y = (id / (_texture.get_width() / _tile_width)) * _tile_width;

        engine::Rectangle result(x, y, _tile_width, _tile_height);
        return result;
    }

} // namespace engine::tilemap
