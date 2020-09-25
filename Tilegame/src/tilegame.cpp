#include "tilegame.hpp"

#include <vector>
#include <memory>
#include <sstream>

#include "engine.hpp"

namespace tilegame
{
    engine::Map *map1;
    std::unique_ptr<engine::Renderer> map1Renderer;

    std::unique_ptr<engine::FreeEntity> entity;
    std::unique_ptr<engine::Player> player;

    void Tilegame::initialize()
    {
        Game::initialize();

        entity = std::unique_ptr<engine::FreeEntity>(new engine::FreeEntity());
        player = std::unique_ptr<engine::Player>(new engine::Player(entity.get(), this->graphicsDevice->getViewport()));

        //glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        window->setPosition(700, 400);
    }

    void Tilegame::loadContent()
    {
        Game::loadContent();

        //map1->loadFromFile("content/world/", "map1.tmx");

        map1 = this->resourceManager->loadResource<engine::Map>("map1", "content/world/map1.tmx", "content/world/");
        map1Renderer = std::make_unique<engine::Renderer>(*map1);
        map1Renderer->initialize();
    }

    void Tilegame::unloadContent()
    {
        //map1Renderer.reset();
    }

    void Tilegame::processInput()
    {
        if (window->isKeyPressed(GLFW_KEY_LEFT))
            player->moveLeft();
        if (window->isKeyPressed(GLFW_KEY_RIGHT))
            player->moveRight();
        if (window->isKeyPressed(GLFW_KEY_UP))
            player->moveUp();
        if (window->isKeyPressed(GLFW_KEY_DOWN))
            player->moveDown();
    }

    void Tilegame::update(const double deltaTime)
    {
        player->update(deltaTime);

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
        map1Renderer->draw(*spriteBatch, player->getCamera());

        frames++;
    }
} // namespace tilegame
