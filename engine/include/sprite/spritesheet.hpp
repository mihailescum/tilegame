#pragma once

#include <unordered_map>
#include <string>

#include "nlohmann/json.hpp"
#include "tileson/tileson.hpp"

#include "core/resource.hpp"
#include "core/resourcemanager.hpp"
#include "core/texture2d.hpp"
#include "core/rectangle.hpp"

#include "sprite/sprite.hpp"

namespace engine::sprite
{
    class SpriteSheet : public engine::Resource
    {
    protected:
        int _tile_width;
        int _tile_height;
        Texture2D *_texture;

        std::unordered_map<std::string, Sprite> _sprites;

    public:
        SpriteSheet() : SpriteSheet(nullptr, 0, 0) {}
        SpriteSheet(engine::Texture2D *texture, int tile_width, int tile_height) : _texture(texture), _tile_width(tile_width), _tile_height(tile_height) {}

        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        void parse(const tson::Tileset &data, ResourceManager &resource_manager);

        engine::Rectangle get_source_rect(unsigned int id) const;

        const Sprite &operator[](std::string name) const { return _sprites.at(name); }
        Sprite &operator[](std::string name) { return _sprites.at(name); }

        const Texture2D &get_texture() const;
        Texture2D &get_texture();

        int get_tile_width() const { return _tile_width; }
        int get_tile_height() const { return _tile_height; }
    };
} // namespace engine::sprite
