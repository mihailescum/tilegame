#include "tilemap/tileset.hpp"

#include "core/circle.hpp"
#include "core/point.hpp"
#include "core/rectangle.hpp"
#include "graphics/sprite.hpp"

namespace engine::tilemap
{
    bool Tileset::load_resource(ResourceManager &resource_manager, va_list args)
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

    void Tileset::unload_resource()
    {
    }

    void Tileset::parse(const tson::Tileset &tson_tileset, ResourceManager &resource_manager)
    {
        SpriteSheet::parse(tson_tileset, resource_manager);

        const int first_gid = tson_tileset.getFirstgid();
        const int last_gid = tson_tileset.getFirstgid() + tson_tileset.getTileCount() - 1;

        // Careful: Due to a bug Tileson generates more tiles than there are between firstgid and lastgid (some are duplicates)
        // This happens because tson::Tileset::generateMissingTiles mixes ids/gids incorrectly
        // _tiles.resize(tson_tiles.size()); WRONG!!
        _tiles.resize(last_gid - first_gid + 1);

        // 'const_cast' is okay, because tson::Tileset::getTiles should have been declared 'const'
        const auto &tson_tiles = const_cast<tson::Tileset &>(tson_tileset).getTiles();
        for (const auto &tson_tile : tson_tiles)
        {
            int gid = tson_tile.getGid();

            Tile tile;
            tile.id = gid - first_gid;
            tile.tileset = this;
            tile.class_type = tson_tile.getClassType();
            // 'const_cast' is okay, because tson::Tile::getProperties should have been declared 'const'
            tile.properties = const_cast<tson::Tile &>(tson_tile).getProperties();

            // 'const_cast' is okay, because tson::Tile::getObjectGroup should have been declared 'const'
            const auto &objectgroup = const_cast<tson::Tile &>(tson_tile).getObjectgroup();
            if (objectgroup.getType() == tson::LayerType::ObjectGroup)
            {
                // 'const_cast' is okay, because tson::Tile::getObjectGroup should have been declared 'const'
                const tson::Object *tson_collision_shape = nullptr;
                for (const auto &o : const_cast<tson::Layer &>(objectgroup).getObjects())
                {
                    if (!tson_collision_shape && o.getClassType() == Tileset::NAME_COLLISION_SHAPE)
                    {
                        tson_collision_shape = &o;
                    }
                }

                if (tson_collision_shape)
                {
                    tile.collision_shape = std::move(parse_shape(*tson_collision_shape));
                }
            }

            // This is a workaround agains the bug mentioned above.
            // tson_tile.getId() refers to the ID used by tiled. If this is less than gid,
            // this means that the current tileset is not the first tileset of the map, but
            // the tson_tile contains the actual Tiled data (and is not automatically generated
            // by Tileson), which is why we keep it. If old_tile.id < 0, then the old tile was
            // still default initialized (as ID >= 1 for all valid tiles) and we overwrite it.
            const auto &old_tile = _tiles[tile.id];
            if (old_tile.id < 0 || tson_tile.getId() < gid)
            {
                _tiles[tile.id] = std::move(tile);
            }
        }
    }

    std::unique_ptr<const Shape> Tileset::parse_shape(const tson::Object &object) const
    {
        const auto &position = object.getPosition();
        const auto &size = object.getSize();

        std::unique_ptr<const Shape> result;
        if (object.isPoint())
        {
            result = std::make_unique<engine::Point>(position.x, position.y);
        }
        else if (object.isEllipse())
        {
            if (std::abs(size.x - size.y) > 1e-4) // We have a true ellipse in this case
            {
                throw "Ellipses are not supported";
            }
            else
            {
                float radius = static_cast<float>(size.x) / 2;
                glm::vec2 center(position.x + radius, position.y + radius);
                result = std::make_unique<engine::Circle>(center, radius);
            }
        }
        else // object is Rectangle
        {
            result = std::make_unique<engine::Rectangle>(position.x, position.y, size.x, size.y);
        }

        return result;
    }

    const Tile *Tileset::get(int id) const
    {
        if (id >= 0 && id < _tiles.size())
        {
            return &_tiles[id];
        }
        else
        {
            return nullptr;
        }
    }
} //
