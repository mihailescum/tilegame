#pragma once

#include <string>

#include <entt/entity/observer.hpp>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class SceneGraph : public System
    {
    private:
        entt::observer _transformation_observer;
        bool _is_first_update = true;

        void update_node_transform(const tilegame::SceneGraphNode &node);

    public:
        SceneGraph(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void unload_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame