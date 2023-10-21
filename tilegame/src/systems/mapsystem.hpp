#pragma once

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class MapSystem : public System
    {
    private:
    public:
        MapSystem(engine::Scene &scene, entt::registry &registry);
    };
} // namespace tilegame