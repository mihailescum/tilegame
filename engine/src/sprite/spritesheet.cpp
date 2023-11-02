#include "sprite/spritesheet.hpp"

#include <string>
#include <vector>

#include "sprite/spritestate.hpp"
#include "sprite/spriteframe.hpp"

namespace engine::sprite
{
    bool SpriteSheet::load_resource(ResourceManager &resource_manager, va_list args)
    {
        pugi::xml_document doc;
        auto result = doc.load_file(_resource_path.c_str());
        if (!result)
        {
            return false;
        }

        auto tileset_node = doc.child("tileset");
        if (!tileset_node)
        {
            return false;
        }

        parse(tileset_node, resource_manager);

        return true;
    }

    void SpriteSheet::unload_resource()
    {
    }

    engine::Rectangle SpriteSheet::get_source_rect(unsigned int id) const
    {
        int x = (id % (_texture->get_width() / _tile_width)) * _tile_width;
        int y = (id / (_texture->get_width() / _tile_width)) * _tile_width;

        engine::Rectangle result(x, y, _tile_width, _tile_height);
        return result;
    }

    void SpriteSheet::parse(const pugi::xml_node &node, ResourceManager &resource_manager)
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
    }
} // namespace engine::sprite