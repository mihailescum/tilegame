#include "core/game.hpp"

#include <memory>

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

        float last_update = glfwGetTime();
        float last_draw = glfwGetTime();
        float delta_update = 0;
        float delta_draw = 0;
        float now = last_update;

        float accumulated_time = _time_step;

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
            draw(_draw_time);
            end_draw(_draw_time);

            glfwSwapBuffers(&_window.native_window());
            last_draw = now;

            _shouldRun = !_window.should_close();
        }

        // Calls unload_content() of derived class
        unload_content();
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

        int graphicsDeviceResult = _graphicsdevice.create();
        if (!graphicsDeviceResult)
        {
            _shouldRun = false;
            return;
        }

        int postprocessor_result = _postprocessor.initialize();
        if (!postprocessor_result)
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

    void Game::begin_update()
    {
    }

    void Game::begin_draw()
    {
        if (_postprocessing_enabled)
        {
            _postprocessor.begin();
        }
    }

    void Game::end_draw(const engine::GameTime &draw_time)
    {
        if (_postprocessing_enabled)
        {
            _postprocessor.end();
            _postprocessor.draw(draw_time);
        }
    }

    void Game::resize(int width, int height)
    {
        Viewport currentViewport = _graphicsdevice.viewport();
        currentViewport.size = glm::ivec2(width, height);
        _graphicsdevice.viewport(currentViewport);
    }
} // namespace engine