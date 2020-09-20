#ifndef __ENGINE_GAME_H__
#define __ENGINE_GAME_H__

#include "window.h"

namespace engine
{
    class Game
    {
    public:
        Game() {}
        ~Game() {}
        void run();

    protected:
        virtual void initialize();
        virtual void loadContent() = 0;
        virtual void unloadContent() = 0;
        virtual void update(const double deltaTime) = 0;
        virtual void draw() = 0;
        Window window;
        double timeStep;

    private:
        bool shouldRun;
    };
} // namespace engine

#endif