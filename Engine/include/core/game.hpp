#pragma once

#include <memory>

#include "core/graphicsdevice.hpp"
#include "core/spritebatch.hpp"
#include "core/window.hpp"
#include "resourcemanager.hpp"

namespace engine
{
    class Game
    {
    private:
        static const int DEFAULT_WINDOW_WIDTH = 800;
        static const int DEFAULT_WINDOW_HEIGHT = 600;

        bool shouldRun;

    protected:
        std::unique_ptr<Window> window;
        std::unique_ptr<GraphicsDevice> graphicsDevice;
        std::unique_ptr<ResourceManager> resourceManager;
        std::unique_ptr<SpriteBatch> spriteBatch;
        double timeStep;

        virtual void initialize();
        virtual void loadContent();
        virtual void unloadContent();
        virtual void processInput() = 0;
        virtual void update(const double deltaTime) = 0;
        virtual void draw() = 0;
        virtual void resize(const int width, const int height);

    public:
        Game() {}
        Game(const Game &game) = delete;
        virtual ~Game();
        
        void run();
    };
} // namespace engine
