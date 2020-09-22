#ifndef __ENGINE_GAME_H__
#define __ENGINE_GAME_H__

#include "graphicsdevice.hpp"
#include "spritebatch.hpp"
#include "window.hpp"

namespace engine
{
    class Game
    {
    public:
        Game() : window(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), graphicsDevice(window), spriteBatch(graphicsDevice) {}
        ~Game() {}
        void run();

    private:
        bool shouldRun;
        static const int DEFAULT_WINDOW_WIDTH = 800;
        static const int DEFAULT_WINDOW_HEIGHT = 600;

    protected:
        virtual void initialize();
        virtual void loadContent() = 0;
        virtual void unloadContent() = 0;
        virtual void update(const double deltaTime) = 0;
        virtual void draw() = 0;
        virtual void resize(const int width, const int height);
        Window window;
        double timeStep;

        GraphicsDevice graphicsDevice;
        SpriteBatch spriteBatch;
    };
} // namespace engine

#endif