#pragma once

#include "entt.hpp"

#include "engine.hpp"

#include "tilegame.hpp"

namespace tilegame
{
    class WorldScene : public engine::Scene
    {
    private:
        std::unique_ptr<engine::Observer> cameraObserver;
        engine::SpriteBatch &spriteBatch;

        void updateCameras();
        void updateMovables(const double deltaTime);
        void handlePlayerInput(const entt::entity &entity, int key, const std::pair<bool, bool> keyPressed);

    public:
        WorldScene(Tilegame &game);

        virtual void initialize() override;
        virtual void loadContent() override;
        virtual void unloadContent() override;
        virtual void processInput() override;
        virtual void update(const double deltaTime) override;
        virtual void draw() override;
    };
} // namespace tilegame