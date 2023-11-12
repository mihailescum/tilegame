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

        double last_update = glfwGetTime();
        double last_draw = glfwGetTime();
        double delta_update = 0;
        double delta_draw = 0;
        double now = last_update;

        double accumulated_time = _time_step;

        // - While window is alive
        while (_shouldRun)
        {

            now = glfwGetTime();
            delta_update = now - last_update;
            accumulated_time += delta_update;

            glfwPollEvents();

            // - Update at 1.0/_time_step FPS
            while (accumulated_time >= _time_step)
            {
                // - Simulate a fixed timestep
                _update_time.elapsed_time = _time_step;
                _update_time.total_elapsed_time += _time_step;

                begin_update();
                update(_update_time);
                end_update();
                last_update = now;

                accumulated_time -= _time_step;
            }

            now = glfwGetTime();
            delta_draw = now - last_draw;
            _draw_time.elapsed_time = delta_draw;
            _draw_time.total_elapsed_time += delta_draw;

            // - Render at maximum possible frames
            begin_draw();
            draw(_draw_time); // - Render function
            end_draw();
            glfwSwapBuffers(&_window.get_native_window());

            last_draw = now;

            _shouldRun = !_window.should_close();
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

        int windowResult = _window.initialize();
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