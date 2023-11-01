#pragma once

#include "pugixml/pugixml.hpp"

#include "core/resource.hpp"
#include "core/resourcemanager.hpp"
#include "core/texture2d.hpp"
#include "core/rectangle.hpp"

#include "sprite/sprite.hpp"

namespace engine::sprite
{
    class SpriteSheet : public engine::Resource
    {
    private:
        int _tile_width;
        int _tile_height;
        Texture2D *_texture;

        void parse(const pugi::xml_node &node, ResourceManager &resource_manager);

    public:
        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        engine::Rectangle get_source_rect(unsigned int id) const;
    };
} // namespace engine::sprite
