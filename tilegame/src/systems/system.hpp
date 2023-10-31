#pragma once

#include <entt/entity/registry.hpp>

#include "engine.hpp"
#include "scenegraphdata.hpp"

namespace tilegame::systems
{
    class System
    {
    protected:
        engine::Scene &_scene;
        entt::registry &_registry;
        tilegame::SceneGraphNode &_scene_graph_root;

    public:
        System(engine::Scene &scene, entt::registry &registry, tilegame::SceneGraphNode &scene_graph_root);
    };
} // namespace tilegame