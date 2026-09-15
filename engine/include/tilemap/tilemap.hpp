#pragma once

#include <vector>
#include <memory>

#include "tileson/tileson.hpp"

#include "core/resource.hpp"
#include "tilemap/tilelayer.hpp"
#include "tilemap/tileset.hpp"
#include "tilemap/tileobject.hpp"
#include "tilemap/tile.hpp"

namespace engine::tilemap
{
    /**
     * @brief A Resource that loads a Tiled map (.tmj, via tileson) into its tile
     * layers, object layers and the set of tilesets it references. Tile layers store
     * only global tile ids (gids); get() resolves a gid across the map's tilesets
     * back to the underlying Tile definition.
     */
    class TileMap : public engine::Resource
    {
    private:
        glm::ivec2 _dimensions;

        std::vector<std::unique_ptr<TileLayer>> _layers;
        std::vector<std::tuple<int, int, const Tileset *>> _tilesets;
        std::vector<std::unique_ptr<TileObject>> _objects;

        void parse_tilesets(const tson::Map &tson_map, ResourceManager &resource_manager);
        void parse_layers(const tson::Map &tson_map, ResourceManager &resource_manager);
        void parse_objectgroup(const tson::Layer &tson_layer, ResourceManager &resource_manager);
        void parse_tilelayer(const tson::Layer &tson_layer, ResourceManager &resource_manager, int z_index);

        int get_gid_at(const tson::Layer &tson_layer, int x, int y) const;
        const Tileset *get_tileset_from_gid(int gid, int &id) const;

    public:
        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;

        /** @brief Resolves a global tile id to its Tile definition in whichever tileset's gid range contains it, or nullptr if no tileset covers it. */
        const Tile *get(int gid) const;

        // TODO return iterators instead
        const std::vector<std::unique_ptr<TileLayer>> &layers() const { return _layers; }
        const std::vector<std::unique_ptr<TileObject>> &objects() const { return _objects; }

        // TODO return only the tilesets, without gids
        /** @brief The map's tilesets as (first_gid, last_gid, tileset) tuples describing the gid range each tileset owns. */
        const auto &tilesets() const { return _tilesets; }
    };
}