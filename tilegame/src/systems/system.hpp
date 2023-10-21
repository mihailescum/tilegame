#pragma once

#include "entt.hpp"

#include "engine.hpp"

namespace tilegame::systems
{
    class System
    {
    protected:
        engine::Scene &_scene;
        entt::registry &_registry;

    public:
        System(engine::Scene &scene, entt::registry &registry);
    };
} // namespace tilegame