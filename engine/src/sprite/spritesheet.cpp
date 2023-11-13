#include "sprite/spritesheet.hpp"

#include <string>
#include <vector>

#include "sprite/spritestate.hpp"
#include "sprite/spriteframe.hpp"

namespace engine::sprite
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

    void SpriteSheet::parse(tson::Tileset &data, ResourceManager &resource_manager)
    {
        _tile_width = data.getTileSize().x;
        _tile_height = data.getTileSize().y;

        const auto texture_path = data.getFullImagePath();
        const auto texture_name = texture_path.filename();
        _texture = resource_manager.load_resource<Texture2D>(texture_name, texture_path);

        const auto &tiles = data.getTiles();
        for (const auto &tile : tiles)
        {
            // const_cast is okay, because the original tiles vector was not const
            const auto &animation = const_cast<tson::Tile &>(tile).getAnimation();

            // Skip if we don't have animation data
            if (animation.size() == 0)
            {
                continue;
            }

            const std::string name = tile.getType();
            Sprite &sprite = _sprites[name];
            sprite.sprite_sheet(this);
            sprite.parse(const_cast<tson::Tile &>(tile));
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
} // namespace engine::sprite