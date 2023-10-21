#include "core/game.hpp"

#include <memory>

#include <GLFW/glfw3.h>

namespace engine
{
    Game::~Game()
    {
        // Calls unloadContent() of Game class
        unloadContent();
    }

    void Game::run()
    {
        shouldRun = true;
        initialize();
        if (shouldRun)
            loadContent();

        double lastTime = glfwGetTime();
        double nowTime = lastTime;
        double accumulatedTime = timeStep;
        double deltaTime = 0;

        // - While window is alive
        while (shouldRun)
        {
            // - Measure time
            lastTime = nowTime;
            nowTime = glfwGetTime();
            deltaTime = (nowTime - lastTime);
            accumulatedTime += deltaTime;

            glfwPollEvents();
            processInput();

            // - Update at X FPS
            while (accumulatedTime >= timeStep)
            {
                update(timeStep); // - Update function
                accumulatedTime -= timeStep;
            }
            // - Render at maximum possible frames
            draw(); // - Render function
            glfwSwapBuffers(window.getGLFWwindow());

            shouldRun = !window.shouldClose();
        }

        // Calls unloadContent() of derived class
        unloadContent();
    }

    void Game::initialize()
    {
        timeStep = 1.0 / 60;

        int windowResult = window.initialize();
        if (!windowResult)
        {
            shouldRun = false;
            return;
        }

        int graphicsDeviceResult = graphicsDevice.create();
        if (!graphicsDeviceResult)
        {
            shouldRun = false;
            return;
        }
    }

    void Game::loadContent()
    {
    }

    void Game::unloadContent()
    {
        resourceManager.unloadResources();
    }

    void Game::resize(int width, int height)
    {
        Viewport currentViewport = graphicsDevice.getViewport();
        currentViewport.width = width;
        currentViewport.height = height;
        graphicsDevice.setViewport(currentViewport);
    }
} // namespace engine