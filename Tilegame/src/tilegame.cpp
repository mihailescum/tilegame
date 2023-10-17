#include "tilegame.hpp"

#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <glm/gtx/transform.hpp>

#include "engine.hpp"

#include "worldscene/worldscene.hpp"

namespace tilegame
{
    void Tilegame::initialize()
    {
        Game::initialize();

        // this->spriteBatch = std::make_unique<engine::SpriteBatch>(*this->graphicsDevice);

        // glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // window->setPosition(700, 400);

        // scene = std::make_unique<worldscene::WorldScene>(*this);
        // scene->initialize();
    }

    void Tilegame::loadContent()
    {
        Game::loadContent();

        // this->spriteBatch->create();
        // scene->loadContent();
    }

    void Tilegame::unloadContent()
    {
        // scene->unloadContent();
        // this->spriteBatch.reset();
    }

    void Tilegame::processInput()
    {
        // scene->processInput();
    }

    void Tilegame::update(const double deltaTime)
    {
        // scene->update(deltaTime);

        timer += deltaTime;
        updates++;
        // - Reset after one second
        if (timer > 1)
        {
            timer--;
            std::stringstream ss;
            ss << "FPS: " << frames << " - UPS: " << updates;
            window->setTitle(ss.str());
            updates = 0, frames = 0;
        }
    }

    void Tilegame::draw()
    {
        graphicsDevice->clear(engine::Color::CornflowerBlue);

        // scene->draw();

        frames++;
    }

    engine::SpriteBatch *Tilegame::getSpriteBatch() const
    {
        return this->spriteBatch.get();
    }
} // namespace tilegame
