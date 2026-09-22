#include "tilegame.hpp"

#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <glm/gtx/transform.hpp>

#include "scenes/worldscene.hpp"

namespace tilegame
{
    void Tilegame::initialize()
    {
        Game::initialize();

        // glfwSetInputMode(window.native_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // window->position(700, 400);

        scene_manager().push<scenes::WorldScene>();
    }

    void Tilegame::load_content()
    {
        Game::load_content();
    }

    void Tilegame::update(const engine::GameTime &update_time)
    {
        scene_manager().update(update_time);

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

        scene_manager().begin_update();
    }

    void Tilegame::end_update()
    {
        scene_manager().end_update();

        Game::end_update();
    }

    void Tilegame::draw(const engine::GameTime &draw_time)
    {
        _graphicsdevice.clear(engine::Color::CORNFLOWER_BLUE);

        scene_manager().draw(draw_time);

        _frames++;
    }

    void Tilegame::begin_draw()
    {
        Game::begin_draw();

        scene_manager().begin_draw();
    }

    void Tilegame::end_draw(const engine::GameTime &draw_time)
    {
        scene_manager().end_draw(draw_time);

        Game::end_draw(draw_time);
    }
} // namespace tilegame
