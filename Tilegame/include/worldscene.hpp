#pragma once

#include "engine.hpp"

namespace tilegame
{
    class WorldScene : public engine::Scene
    {
    private:
        std::unique_ptr<engine::SpriteBatch> spriteBatch;

    public:
        WorldScene(engine::Game &game);

        virtual void initialize() override;
        virtual void loadContent() override;
        virtual void unloadContent() override;
        virtual void processInput() override;
        virtual void update(const double deltaTime) override;
        virtual void draw() override;
    };
} // namespace tilegame