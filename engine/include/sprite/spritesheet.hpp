#pragma once

#include <unordered_map>
#include <string>

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

        std::unordered_map<std::string, Sprite> _sprites;

        void parse(const pugi::xml_node &node, ResourceManager &resource_manager);

    public:
        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        engine::Rectangle get_source_rect(unsigned int id) const;

        Sprite &operator[](std::string name) { return _sprites[name]; }

        const Texture2D *get_texture() const { return _texture; }
        Texture2D *get_texture() { return _texture; }
    };
} // namespace engine::sprite
