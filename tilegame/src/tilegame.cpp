#include "tilegame.hpp"

#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <glm/gtx/transform.hpp>

#include "engine.hpp"

// #include "worldscene/worldscene.hpp"

namespace tilegame
{
    void Tilegame::initialize()
    {
        Game::initialize();

        _spritebatch.create();

        // glfwSetInputMode(window.get_native_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // window->set_position(700, 400);

        // scene = std::make_unique<worldscene::WorldScene>(*this);
        // scene->initialize();
    }

    void Tilegame::load_content()
    {
        Game::load_content();

        _worldscene.load_content();
    }

    void Tilegame::unload_content()
    {
        _worldscene.unload_content();
        // this->spriteBatch.reset();
    }

    void Tilegame::update(const double deltaTime)
    {
        _worldscene.update(deltaTime);

        _timer += deltaTime;
        _updates++;
        // - Reset after one second
        if (_timer > 1)
        {
            _timer--;
            std::stringstream ss;
            ss << "FPS: " << _frames << " - UPS: " << _updates;
            window.set_title(ss.str());
            _updates = 0, _frames = 0;
        }
    }

    void Tilegame::draw()
    {
        _graphics_device.clear(engine::Color::CORNFLOWER_BLUE);

        _worldscene.draw();

        _frames++;
    }
} // namespace tilegame
