#pragma once

#include "graphicsdevice.hpp"
#include "window.hpp"
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
        Window window;
        GraphicsDevice graphicsDevice;
        ResourceManager resourceManager;
        double timeStep;

        virtual void initialize();
        virtual void loadContent();
        virtual void unloadContent();
        virtual void processInput() = 0;
        virtual void update(double deltaTime) = 0;
        virtual void draw() = 0;
        virtual void resize(int width, int height);

    public:
        Game(int windowWidth = DEFAULT_WINDOW_WIDTH, int windowHeight = DEFAULT_WINDOW_HEIGHT) : window(windowWidth, windowHeight), graphicsDevice(window) {}
        Game(const Game &game) = delete;
        virtual ~Game();

        const Window &getWindow() const { return window; }
        const GraphicsDevice &getGraphicsDevice() const { return graphicsDevice; }
        ResourceManager &getResourceManager() { return resourceManager; }

        void run();
    };
} // namespace engine
