#include "core/game.hpp"

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
            this->shouldRun = !window->shouldClose();

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
            glfwSwapBuffers(window->getGLFWwindow());
            glfwPollEvents();
        }
        unloadContent();
    }

    void Game::initialize()
    {
        timeStep = 1.0 / 60;

        this->window = std::unique_ptr<Window>(new Window(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT));
        int windowResult = window->initialize();
        if (!windowResult)
        {
            this->shouldRun = false;
            return;
        }
        
        this->graphicsDevice = std::unique_ptr<GraphicsDevice>(new GraphicsDevice(*this->window));
        int graphicsDeviceResult = graphicsDevice->create();
        if (!graphicsDeviceResult)
        {
            this->shouldRun = false;
            return;
        }

        this->spriteBatch = std::unique_ptr<SpriteBatch>(new SpriteBatch(*this->graphicsDevice));
    }

    void Game::loadContent()
    {
        this->spriteBatch->create();
    }

    void Game::unloadContent()
    {
        this->spriteBatch.reset();
        this->graphicsDevice.reset();
    }

    void Game::resize(const int width, const int height) {
        Viewport currentViewport = graphicsDevice->getViewport();
        currentViewport.width = width;
        currentViewport.height = height;
        graphicsDevice->setViewport(currentViewport);
    }
} // namespace engine