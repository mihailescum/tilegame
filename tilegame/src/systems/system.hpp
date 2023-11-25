#pragma once

#include <entt/entity/registry.hpp>

#include "engine.hpp"
#include "scenegraphdata.hpp"

namespace tilegame::systems
{
    class System
    {
    protected:
        tilegame::Scene &_scene;
        entt::registry &_registry;

    public:
        System(tilegame::Scene &scene, entt::registry &registry);
        virtual ~System() = 0;
    };
} // namespace tilegame