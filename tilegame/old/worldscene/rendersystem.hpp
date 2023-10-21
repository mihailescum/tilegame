#pragma once

#include "entt.hpp"

#include "engine.hpp"

namespace tilegame::worldscene
{
    class WorldScene;

    class RenderSystem
    {
    private:
        entt::registry &registry;
        engine::SpriteBatch &spriteBatch;

        void drawTileLayer(const entt::entity &entity) const;
        void drawSprite(const entt::entity &entity) const;

    public:
        RenderSystem(WorldScene &scene, engine::SpriteBatch &spriteBatch);

        void draw() const;
        void initialize();
    };
} // namespace engine
