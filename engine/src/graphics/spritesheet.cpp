#include "graphics/spritesheet.hpp"

#include <string>
#include <vector>

#include "graphics/spritestate.hpp"
#include "graphics/spriteframe.hpp"

namespace engine::graphics
{
    bool SpriteSheet::load_resource(ResourceManager &resource_manager, va_list args)
    {
        tson::Json11 parser;

        std::filesystem::path working_dir = _resource_path.parent_path();

        if (parser.parse(_resource_path))
        {
            tson::Tileset data;
            if (data.parse(parser, nullptr))
            {
                parse(data, resource_manager);
            }
        }

        return false;
    }

    void SpriteSheet::unload_resource()
    {
    }

    void SpriteSheet::parse(const tson::Tileset &data, ResourceManager &resource_manager)
    {
        _tile_width = data.getTileSize().x;
        _tile_height = data.getTileSize().y;

        const auto texture_path = data.getFullImagePath();
        const auto texture_name = texture_path.filename();
        _texture = resource_manager.load_resource<Texture2D>(texture_name, texture_path);

        // 'const_cast' is okay, because tson::Tileset::getTiles should have been declared 'const'
        const auto &tson_tiles = const_cast<tson::Tileset &>(data).getTiles();
        for (const auto &tson_tile : tson_tiles)
        {
            // 'const_cast' is okay, because tson::Tile::getAnimation should have been declared 'const'
            const auto &animation = const_cast<tson::Tile &>(tson_tile).getAnimation();

            // Skip if we don't have animation data
            if (animation.size() == 0)
            {
                continue;
            }

            const std::string name = tson_tile.getType();
            Sprite &sprite = _sprites[name];
            sprite.sprite_sheet(this);
            sprite.parse(tson_tile);
        }
    }

    engine::Rectangle SpriteSheet::source_rect(unsigned int id) const
    {
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