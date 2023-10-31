#pragma once

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class RenderSystem : public System
    {
    private:
        engine::SpriteBatch &_spritebatch;

    public:
        RenderSystem(tilegame::Scene &scene, entt::registry &registry, engine::SpriteBatch &spritebatch);

        void draw();
    };
} // namespace tilegame