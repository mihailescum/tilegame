#pragma once

#include "engine.hpp"

#include "tilegame.hpp"

namespace tilegame
{
    class WorldScene : public engine::Scene
    {
    private:
        std::unique_ptr<engine::Observer> cameraObserver;
        engine::SpriteBatch &spriteBatch;

    public:
        WorldScene(Tilegame &game);

        virtual void initialize() override;
        virtual void loadContent() override;
        virtual void unloadContent() override;
        virtual void processInput() override;
        virtual void update(const double deltaTime) override;
        virtual void draw() override;

        void updateCameras();
    };
} // namespace tilegame