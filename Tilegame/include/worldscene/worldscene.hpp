#pragma once

#include "entt.hpp"

#include "engine.hpp"

#include "tilegame.hpp"
#include "worldscene/contentsystem.hpp"
#include "worldscene/inputsystem.hpp"
#include "worldscene/camerasystem.hpp"
#include "worldscene/movementsystem.hpp"
#include "worldscene/rendersystem.hpp"

namespace tilegame::worldscene
{
    //class Tilegame;

    class WorldScene : public engine::Scene
    {
    private:
        engine::SpriteBatch &spriteBatch;

        std::unique_ptr<ContentSystem> contentSystem;
        std::unique_ptr<InputSystem> inputSystem;
        std::unique_ptr<CameraSystem> cameraSystem;
        std::unique_ptr<MovementSystem> movementSystem;
        std::unique_ptr<RenderSystem> renderSystem;

        void createSystems();

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