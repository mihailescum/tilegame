#include "sprite/spritesheet.hpp"

#include <string>
#include <vector>

#include "sprite/spritestate.hpp"
#include "sprite/spriteframe.hpp"

namespace engine::sprite
{
    bool SpriteSheet::load_resource(ResourceManager &resource_manager, va_list args)
    {
        tson::NlohmannJson parser;

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

        const auto &tiles = const_cast<tson::Tileset &>(data).getTiles();
        for (const auto &tile : tiles)
        {
            const auto &animation = const_cast<tson::Tile &>(tile).getAnimation();

            // Skip if we don't have animation data
            if (animation.size() == 0)
            {
                continue;
            }

            const std::string name = tile.getType();
            Sprite &sprite = _sprites[name];
            sprite.set_sprite_sheet(this);
            sprite.parse(tile);
        }
    }

    engine::Rectangle SpriteSheet::get_source_rect(unsigned int id) const
    {
        int x = (id % (_texture->get_width() / _tile_width)) * _tile_width;
        int y = (id / (_texture->get_width() / _tile_width)) * _tile_width;

        engine::Rectangle result(x, y, _tile_width, _tile_height);
        return result;
    }

    const Texture2D &SpriteSheet::get_texture() const
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

    Texture2D &SpriteSheet::get_texture()
    {
        return const_cast<Texture2D &>(const_cast<const SpriteSheet *>(this)->get_texture());
    }

    /*void SpriteSheet::parse(const pugi::xml_node &node, ResourceManager &resource_manager)
         {
             _tile_width = node.attribute("tilewidth").as_int();
             _tile_height = node.attribute("tileheight").as_int();

             int columns = node.attribute("columns").as_int();

             const auto image_node = node.child("image");
             const std::filesystem::path image_source = _resource_path.parent_path() / image_node.attribute("source").as_string();
             const std::string image_name = image_source.filename();

             _texture = resource_manager.load_resource<engine::Texture2D>(image_name, image_source);

             const auto tile_nodes = node.children("tile");
             for (const auto &tile_node : tile_nodes)
             {
                 // Skip if we don't have animation data
                 const auto &animation_node = tile_node.child("animation");
                 if (!animation_node)
                 {
                     continue;
                 }

                 const std::string name = tile_node.attribute("type").as_string();
                 Sprite &sprite = _sprites[name];
                 sprite.set_sprite_sheet(this);
                 sprite.parse(tile_node);
             }
         }*/
} // namespace engine::sprite