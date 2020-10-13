#pragma once

#include "entt.hpp"

#include "engine.hpp"

namespace tilegame::worldscene
{
    class RenderSystem
    {
    private:
        entt::registry &registry;
        engine::SpriteBatch &spriteBatch;

        void drawTileLayer(const entt::entity &entity) const;
        void drawSprite(const entt::entity &entity) const;

    public:
        RenderSystem(engine::Scene &scene, engine::SpriteBatch &spriteBatch) : registry(scene.getRegistry()), spriteBatch(spriteBatch) {}

        void draw() const;
        void initialize();
    };
} // namespace engine
