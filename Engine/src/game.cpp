#include "game.h"

#include <GLFW/glfw3.h>

namespace engine
{
    void Game::run()
    {
        this->shouldRun = true;
        initialize();
        if (this->shouldRun)
            loadContent();

        double lastTime = glfwGetTime();
        double nowTime = lastTime;
        double accumulatedTime = 0;
        double deltaTime = 0;

        // - While window is alive
        while (this->shouldRun)
        {
            this->shouldRun = !window.shouldClose();

            // - Measure time
            lastTime = nowTime;
            nowTime = glfwGetTime();
            deltaTime = (nowTime - lastTime);
            accumulatedTime += deltaTime;

            // - Update at X FPS
            while (accumulatedTime >= timeStep)
            {
                update(timeStep); // - Update function
                accumulatedTime -= timeStep;
            }
            // - Render at maximum possible frames
            draw(); // - Render function
            glfwSwapBuffers(window.getGLFWwindow());
            glfwPollEvents();
        }
        unloadContent();
    }

    void Game::initialize()
    {
        timeStep = 1.0 / 60;
        int windowResult = window.initialize(1280, 720);
        if (!windowResult)
        {
            this->shouldRun = false;
            return;
        }
    }
} // namespace engine