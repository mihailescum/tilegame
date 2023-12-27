#pragma once

#include <string>
#include <vector>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class Map : public System
    {
    private:
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