#pragma once

#include "entt.hpp"

#include "engine.hpp"

namespace tilegame::worldscene
{
    class MovementSystem
    {
    private:
        entt::registry &registry;

    public:
        MovementSystem(engine::Scene &scene) : registry(scene.getRegistry()) {}
        void initialize();
        void update(const double deltaTime);
    };
} // namespace tilegame::worldscene