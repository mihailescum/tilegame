#pragma once

#include <vector>

#include "core/texture2d.hpp"
#include "core/rectangle.hpp"

namespace engine::tilemap
{
    class Tileset
    {
    private:
        engine::Texture2D &_texture;
        unsigned int _first_GID;
        unsigned int _last_GID;

        int _tile_width;
        int _tile_height;

    public:
        Tileset(engine::Texture2D &texture, unsigned int first_GID, unsigned int last_GID, int tile_width, int tile_height);

        // engine::Texture2D &get_texture() { return _texture; };
        const engine::Texture2D &get_texture() const { return _texture; };

        int get_tile_width() const { return _tile_width; }
        int get_tile_height() const { return _tile_height; }
        bool has_tile(unsigned int id) const;
        engine::Rectangle get_source_rect(unsigned int id) const;
    };
}