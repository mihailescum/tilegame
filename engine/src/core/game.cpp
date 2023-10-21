#include "core/game.hpp"

#include <memory>

#include <GLFW/glfw3.h>

namespace engine
{
    Game::~Game()
    {
        // Calls unload_content() of Game class
        unload_content();
    }

    void Game::run()
    {
        _shouldRun = true;
        initialize();
        if (_shouldRun)
            load_content();

        double lastTime = glfwGetTime();
        double nowTime = lastTime;
        double accumulatedTime = _time_step;
        double deltaTime = 0;

        // - While window is alive
        while (_shouldRun)
        {
            // - Measure time
            lastTime = nowTime;
            nowTime = glfwGetTime();
            deltaTime = (nowTime - lastTime);
            accumulatedTime += deltaTime;

            glfwPollEvents();

            // - Update at X FPS
            while (accumulatedTime >= _time_step)
            {
                begin_update();
                update(_time_step);
                end_update();
                accumulatedTime -= _time_step;
            }
            // - Render at maximum possible frames
            begin_draw();
            draw(); // - Render function
            end_draw();
            glfwSwapBuffers(window.get_native_window());

            _shouldRun = !window.should_close();
        }

        // Calls unload_content() of derived class
        unload_content();
    }

    void Game::begin_update()
    {
    }

    void Game::initialize()
    {
        _time_step = 1.0 / 60;

        int windowResult = window.initialize();
        if (!windowResult)
        {
            _shouldRun = false;
            return;
        }

        int graphicsDeviceResult = _graphics_device.create();
        if (!graphicsDeviceResult)
        {
            _shouldRun = false;
            return;
        }
    }

    void Game::load_content()
    {
    }

    void Game::unload_content()
    {
        _resource_manager.unload_resources();
    }

    void Game::resize(int width, int height)
    {
        Viewport currentViewport = _graphics_device.get_viewport();
        currentViewport.width = width;
        currentViewport.height = height;
        _graphics_device.set_viewport(currentViewport);
    }
} // namespace engine