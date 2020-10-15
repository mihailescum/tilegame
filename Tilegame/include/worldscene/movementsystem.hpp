#pragma once

#include "entt.hpp"

#include "engine.hpp"

namespace tilegame::worldscene
{
    class WorldScene;

    class MovementSystem
    {
    private:
        entt::registry &registry;

    public:
        MovementSystem(WorldScene &scene);
        void initialize();
        void update(const double deltaTime);

        void setPosition(const entt::entity &entity, const float x, const float y);
    };
} // namespace tilegame::worldscene