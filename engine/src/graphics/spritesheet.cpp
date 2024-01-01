#include "graphics/spritesheet.hpp"

#include <string>
#include <vector>

#include "graphics/spritestate.hpp"
#include "graphics/spriteframe.hpp"

namespace engine::graphics
{
    engine::Rectangle SpriteSheet::source_rect(int id) const
    {
        assert(id >= 0);

        int x = (id % (_texture->width() / _tile_width)) * _tile_width;
        int y = (id / (_texture->width() / _tile_width)) * _tile_width;

        engine::Rectangle result(x, y, _tile_width, _tile_height);
        return result;
    }

    const Texture2D &SpriteSheet::texture() const
    {
        if (_texture)
        {
            return *_texture;
        }
        else
        {
            throw "No texture";
        }
    }

    Texture2D &SpriteSheet::texture()
    {
        return const_cast<Texture2D &>(const_cast<const SpriteSheet *>(this)->texture());
    }
} // namespace engine::graphics