#include "tilegame.hpp"

#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <glm/gtx/transform.hpp>

// #include "worldscene/worldscene.hpp"

namespace tilegame
{
    void Tilegame::initialize()
    {
        Game::initialize();

        _spritebatch.create();

        // glfwSetInputMode(window.native_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // window->position(700, 400);

        _worldscene.initialize();
    }

    void Tilegame::load_content()
    {
        Game::load_content();

        _worldscene.load_content();
    }

    void Tilegame::unload_content()
    {
        Game::unload_content();

        _worldscene.unload_content();
    }

    void Tilegame::update(const engine::GameTime &update_time)
    {
        _worldscene.update(update_time);

        _timer += update_time.elapsed_time;
        _updates++;
        // - Reset after one second
        if (_timer > 1)
        {
            _timer--;
            std::stringstream ss;
            ss << "FPS: " << _frames << " - UPS: " << _updates;
            _window.title(ss.str());
            _updates = 0, _frames = 0;
        }
    }

    void Tilegame::begin_update()
    {
        Game::begin_update();
    }

    void Tilegame::end_update()
    {
        _worldscene.end_update();

        Game::end_update();
    }

    void Tilegame::draw(const engine::GameTime &draw_time)
    {
        _graphicsdevice.clear(engine::Color::CORNFLOWER_BLUE);

        _worldscene.draw(draw_time);

        _frames++;
    }

    void Tilegame::begin_draw()
    {
        Game::begin_draw();
    }

    void Tilegame::end_draw(const engine::GameTime &draw_time)
    {
        Game::end_draw(draw_time);
    }
} // namespace tilegame
