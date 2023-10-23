#pragma once

#include <memory>
#include "entity/registry.hpp"

#include "engine.hpp"

namespace tilegame::worldscene
{
    class WorldScene;
    class SpriteSystem
    {
    private:
        WorldScene &worldscene;
        entt::registry &registry;

        std::unique_ptr<engine::Observer> spriteInfoObserver;

    public:
        SpriteSystem(WorldScene &worldscene);

        void initialize();
        void update();
    };
} // namespace tilegame::worldscene