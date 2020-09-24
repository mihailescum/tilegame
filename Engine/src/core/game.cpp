#include "core/game.hpp"

#include <memory>

#include <GLFW/glfw3.h>

namespace engine
{
    Game::~Game() {
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
        unloadContent();
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
        this->spriteBatch = std::make_unique<SpriteBatch>(*this->graphicsDevice);
    }

    void Game::loadContent()
    {
        this->spriteBatch->create();
    }

    void Game::unloadContent()
    {
        this->spriteBatch.reset();
        this->resourceManager.reset();
        this->graphicsDevice.reset();
    }

    void Game::resize(const int width, const int height) {
        Viewport currentViewport = graphicsDevice->getViewport();
        currentViewport.width = width;
        currentViewport.height = height;
        graphicsDevice->setViewport(currentViewport);
    }
} // namespace engine