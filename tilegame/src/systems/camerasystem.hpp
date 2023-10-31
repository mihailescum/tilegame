#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class CameraSystem : public System
    {
    public:
        CameraSystem(engine::Scene &scene, entt::registry &registry, tilegame::SceneGraphNode &scene_graph_root);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame