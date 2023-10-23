#pragma once

#include "entity/registry.hpp"

#include "engine.hpp"

namespace tilegame::worldscene
{
    class WorldScene;
    class AnimationSystem
    {
    private:
        WorldScene &worldscene;
        entt::registry &registry;

        void updateComponents(const double deltaTime);
        void updateVisibleSprites();

    public:
        AnimationSystem(WorldScene &worldscene);

        void initialize();
        void update(const double deltaTime);
    };
}