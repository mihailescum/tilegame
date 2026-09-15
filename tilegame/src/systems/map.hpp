#pragma once

#include <string>
#include <vector>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Loads the tile map resource and populates the registry from it.
     *
     * Runs once on load_content: parses the TileMap resource and, for each
     * tileset/layer/object it contains, creates the corresponding entities
     * (TileMap, TileLayer with per-tile collision shapes, Tileset, and sprite
     * entities with Transform/Sprite/Animation/Collider, optionally a
     * ScriptLoader when the map object references a Lua script).
     */
    class Map : public System
    {
    private:
        // Names of the Tiled custom properties used to look up an animated tile's initial
        // state and an object's attached Lua script path, respectively.
        inline static const std::string FIELD_STATE = "state";
        inline static const std::string FIELD_SCRIPT = "script";

        const entt::entity create_map_entity(const engine::tilemap::TileMap &map);
        const entt::entity create_layer_entity(const engine::tilemap::TileLayer &layer, const engine::tilemap::TileMap &map);
        const entt::entity create_tileset_entity(const engine::tilemap::Tileset &tileset);
        const entt::entity create_sprite_entity(const engine::tilemap::TileObject &object, const engine::tilemap::TileMap &map);

    public:
        Map(tilegame::Scene &scene, entt::registry &registry);

        void load_content();
    };
} // namespace tilegame