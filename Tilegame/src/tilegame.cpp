#include "tilegame.hpp"

#include <vector>
#include <time.h>

#include <sstream>

#include "engine.hpp"

namespace tilegame
{
    engine::Texture2D test;

    void Tilegame::initialize()
    {
        Game::initialize();

        //glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetWindowPos(window.getGLFWwindow(), 700, 400);
    }

    void Tilegame::loadContent()
    {
        test.loadTexture("content/textures/current_demo.png", GL_RGBA, false);
    }

    void Tilegame::unloadContent()
    {
    }

    void Tilegame::update(const double deltaTime)
    {
        timer += deltaTime;
        updates++;

        // - Reset after one second
        if (timer > 1)
        {
            timer--;
            std::stringstream ss;
            ss << "FPS: " << frames << " - UPS: " << updates;
            window.setTitle(ss.str());
            updates = 0, frames = 0;
        }
    }

    void Tilegame::draw()
    {
        graphicsDevice.clear(engine::Color::CornflowerBlue);

        spriteBatch.begin();
        engine::Rectangle source(0, 0, 900, 900);
        spriteBatch.draw(test, engine::Rectangle(100, 100, 100, 100), &source, engine::Color::White);
        spriteBatch.end();

        frames++;
    }
} // namespace tilegame
