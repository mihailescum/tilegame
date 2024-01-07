#pragma once

#include <unordered_map>
#include <string>

#include <glm/glm.hpp>

#include "tileson/tileson.hpp"

#include "core/resource.hpp"
#include "core/resourcemanager.hpp"
#include "core/texture2d.hpp"
#include "core/rectangle.hpp"

#include "graphics/sprite.hpp"

namespace engine::graphics
{
    class SpriteSheet : public Resource
    {
    protected:
        glm::ivec2 _tile_dimensions;
        Texture2D *_texture;

        std::unordered_map<std::string, Sprite> _sprites;

    public:
        SpriteSheet() : SpriteSheet(nullptr, glm::ivec2(0)) {}
        SpriteSheet(engine::Texture2D *texture, const glm::ivec2 &tile_dimensions) : _texture(texture), _tile_dimensions(tile_dimensions) {}

        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        void parse(const tson::Tileset &tson_tileset, ResourceManager &resource_manager);
        engine::Rectangle source_rect(int id) const;

        const Sprite &operator[](std::string name) const { return _sprites.at(name); }
        Sprite &operator[](std::string name) { return _sprites.at(name); }

        const Texture2D &texture() const;
        Texture2D &texture();

        const glm::ivec2 &tile_dimensions() const { return _tile_dimensions; }
    };
} // namespace engine::graphics
