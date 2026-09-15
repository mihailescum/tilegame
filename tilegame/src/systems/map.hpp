#pragma once

#include <string>
#include <vector>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Loads the world's maps and populates the registry from them.
     *
     * Runs once on load_content: parses the World resource (a set of TileMap
     * resources positioned relative to one another) and, for each map's
     * tileset/layer/object, creates the corresponding entities (TileMap,
     * TileLayer with per-tile collision shapes, Tileset, and sprite entities
     * with Transform/Sprite/Animation/Collider, optionally a ScriptLoader
     * when the map object references a Lua script). Every entity created for
     * a map is offset by that map's position within the world, so multiple
     * maps render glued together at the right world-space coordinates.
     */
    class Map : public System
    {
    private:
        // Names of the Tiled custom properties used to look up an animated tile's initial
        // state and an object's attached Lua script path, respectively.
        inline static const std::string FIELD_STATE = "state";
        inline static const std::string FIELD_SCRIPT = "script";

        const entt::entity create_map_entity(const engine::tilemap::TileMap &map, const glm::vec2 &position);
        const entt::entity create_layer_entity(const engine::tilemap::TileLayer &layer, const engine::tilemap::TileMap &map, const glm::vec2 &position);
        const entt::entity create_tileset_entity(const engine::tilemap::Tileset &tileset);
        const entt::entity create_sprite_entity(const engine::tilemap::TileObject &object, const engine::tilemap::TileMap &map, const glm::vec2 &position);

    public:
        Map(tilegame::Scene &scene, entt::registry &registry);

        void load_content();
    };
} // namespace tilegame