#pragma once

#include "entt.hpp"

#include "core/spritebatch.hpp"
#include "scene/scene.hpp"
#include "scene/components/cameracomponent.hpp"

namespace engine
{
    class WorldRenderSystem
    {
    private:
        entt::registry &registry;

        void drawTileLayer(SpriteBatch &spriteBatch, const entt::entity &entity) const;
        void drawSprite(SpriteBatch &spriteBatch, const entt::entity &entity) const;

    public:
        WorldRenderSystem(Scene &scene) : registry(scene.getRegistry()) {}

        void draw(SpriteBatch &spriteBatch, const CameraComponent &camera) const;
        void initialize();
    };
} // namespace engine
