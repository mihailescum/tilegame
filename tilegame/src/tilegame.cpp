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

        spriteBatch.create();

        // glfwSetInputMode(window.get_native_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // window->set_position(700, 400);

        // scene = std::make_unique<worldscene::WorldScene>(*this);
        // scene->initialize();
    }

    void Tilegame::load_content()
    {
        Game::load_content();

        worldScene.load_content();
    }

    void Tilegame::unload_content()
    {
        worldScene.unload_content();
        // this->spriteBatch.reset();
    }

    void Tilegame::update(const double deltaTime)
    {
        worldScene.update(deltaTime);

        timer += deltaTime;
        updates++;
        // - Reset after one second
        if (timer > 1)
        {
            timer--;
            std::stringstream ss;
            ss << "FPS: " << frames << " - UPS: " << updates;
            window.set_title(ss.str());
            updates = 0, frames = 0;
        }
    }

    void Tilegame::draw()
    {
        _graphics_device.clear(engine::Color::CORNFLOWER_BLUE);

        worldScene.draw();

        frames++;
    }
} // namespace tilegame
