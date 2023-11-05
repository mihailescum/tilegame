#pragma once

#include <string>
#include <vector>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class MapSystem : public System
    {
    private:
        const entt::entity create_map_entity(const engine::tilemap::TileMap &map);
        const entt::entity create_layer_entity(const engine::tilemap::TileLayer &layer, const std::vector<entt::entity> &tileset_entities);
        const entt::entity create_tileset_entity(const engine::tilemap::Tileset &tileset);

    public:
        MapSystem(tilegame::Scene &scene, entt::registry &registry);

        void load_content();
    };
} // namespace tilegame