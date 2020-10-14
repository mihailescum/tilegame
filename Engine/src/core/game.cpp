#include "core/game.hpp"

#include <memory>

#include <GLFW/glfw3.h>

namespace engine
{
    Game::~Game()
    {
        // Calls unloadContent() of Game class
        this->unloadContent();
    }

    void Game::run()
    {
        this->shouldRun = true;
        initialize();
        if (this->shouldRun)
            loadContent();

        double lastTime = glfwGetTime();
        double nowTime = lastTime;
        double accumulatedTime = timeStep;
        double deltaTime = 0;

        // - While window is alive
        while (this->shouldRun)
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
            glfwSwapBuffers(window->getGLFWwindow());

            this->shouldRun = !window->shouldClose();
        }

        // Calls unloadContent() of derived class
        this->unloadContent();
    }

    void Game::initialize()
    {
        timeStep = 1.0 / 60;

        int width = DEFAULT_WINDOW_WIDTH;
        int height = DEFAULT_WINDOW_HEIGHT;
        this->window = std::make_unique<Window>(width, height);

        int windowResult = window->initialize();
        if (!windowResult)
        {
            this->shouldRun = false;
            return;
        }

        this->graphicsDevice = std::make_unique<GraphicsDevice>(*this->window);
        int graphicsDeviceResult = graphicsDevice->create();
        if (!graphicsDeviceResult)
        {
            this->shouldRun = false;
            return;
        }

        this->resourceManager = std::make_unique<ResourceManager>();
    }

    void Game::loadContent()
    {
    }

    void Game::unloadContent()
    {
        this->resourceManager.reset();
        this->graphicsDevice.reset();
    }

    void Game::resize(const int width, const int height)
    {
        Viewport currentViewport = graphicsDevice->getViewport();
        currentViewport.width = width;
        currentViewport.height = height;
        graphicsDevice->setViewport(currentViewport);
    }

    const Window *Game::getWindow() const {
        return this->window.get();
    }

    const GraphicsDevice *Game::getGraphicsDevice() const {
        return this->graphicsDevice.get();
    }

    ResourceManager *Game::getResourceManager() const {
        return this->resourceManager.get();
    }
} // namespace engine