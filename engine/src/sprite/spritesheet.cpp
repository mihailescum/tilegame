#include "sprite/spritesheet.hpp"

#include <tmxlite/Map.hpp>
#include <tmxlite/Tileset.hpp>

namespace pugi
{
    class xml_document;
}

namespace engine::sprite
{
    bool SpriteSheet::load_resource(ResourceManager &resource_manager, va_list args)
    {
        // A placeholder map
        tmx::Map _;
        tmx::Tileset tileset;

        pugi::xml_document doc;
        auto result = doc.load_file(_resource_path);
        if (!result)
        {
            return false;
        }

        auto tileset_node = doc.child("tileset");
        if (!tileset_node)
        {
            return false;
        }

        return true;
    }
} // namespace engine::sprite
