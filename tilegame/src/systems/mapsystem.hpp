#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class MapSystem : public System
    {
    private:
    public:
        MapSystem(engine::Scene &scene, entt::registry &registry, tilegame::SceneGraphNode &scene_graph_root);

        const entt::entity create_map_entity_from_file(const std::string name, const std::string path);
    };
} // namespace tilegame