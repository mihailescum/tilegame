#pragma once

#include <vector>
#include <memory>

#include "tileson/tileson.hpp"

#include "core/texture2d.hpp"
#include "core/shape.hpp"
#include "core/rectangle.hpp"
#include "core/resource.hpp"
#include "graphics/spritesheet.hpp"
#include "tilemap/tile.hpp"

namespace engine::tilemap
{
    /**
     * @brief Extends SpriteSheet with per-tile gameplay metadata parsed from Tiled:
     * custom class/properties and an optional collision shape per tile, plus an
     * optional luminosity texture (used for day/night lighting blending) alongside
     * the sheet's base color texture.
     */
    class Tileset : public engine::graphics::SpriteSheet
    {
    private:
        inline static const std::string NAME_COLLISION_SHAPE = "collision";
        inline static const std::string NAME_IMAGE_LUMINOSITY = "image_luminosity";

        std::vector<Tile> _tiles;
        std::unique_ptr<const Shape> parse_shape(const tson::Object &object) const;
        engine::Texture2D *_luminosity_texture;

    public:
        Tileset() : _luminosity_texture(nullptr), SpriteSheet() {}

        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        void parse(const tson::Tileset &tson_tileset, ResourceManager &resource_manager);
        /** @brief Looks up a Tile by its id local to this tileset (not a map-wide gid), or nullptr if out of range. */
        const Tile *get(int id) const;

        const Texture2D &luminosity_texture() const;
        Texture2D &luminosity_texture();
    };
}