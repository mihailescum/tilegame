#ifndef __ENGINE_GAME_H__
#define __ENGINE_GAME_H__

#include <memory>

#include "core/graphicsdevice.hpp"
#include "core/spritebatch.hpp"
#include "core/window.hpp"

namespace engine
{
    class Game
    {
    public:
        Game() {}
        Game(const Game& game) = delete;
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
        std::unique_ptr<Window> window;
        double timeStep;

        std::unique_ptr<GraphicsDevice> graphicsDevice;
        std::unique_ptr<SpriteBatch> spriteBatch;
    };
} // namespace engine

#endif