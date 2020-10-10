#include "tilegame.hpp"

#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <glm/gtx/transform.hpp>

#include "engine.hpp"

#include "worldscene.hpp"

namespace tilegame
{
    std::unique_ptr<WorldScene> scene;

    void Tilegame::initialize()
    {
        Game::initialize();

        //entity = std::make_unique<engine::FreeEntity>();
        //player = std::make_unique<engine::Player>(entity.get(), this->graphicsDevice->getViewport());

        //glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        window->setPosition(700, 400);

        scene = std::make_unique<WorldScene>(*this);
        scene->initialize();
    }

    void Tilegame::loadContent()
    {
        Game::loadContent();

        scene->loadContent();

        //playerCharacter = this->resourceManager->loadResource<engine::Character>("playerCharacter", "content/characters/player.chr");
    }

    void Tilegame::unloadContent()
    {
        //map1Renderer.reset();
        scene->unloadContent();
    }

    void Tilegame::processInput()
    {
        scene->processInput();
        /*if (window->isKeyPressed(GLFW_KEY_LEFT))
            player->moveLeft();
        if (window->isKeyPressed(GLFW_KEY_RIGHT))
            player->moveRight();
        if (window->isKeyPressed(GLFW_KEY_UP))
            player->moveUp();
        if (window->isKeyPressed(GLFW_KEY_DOWN))
            player->moveDown();*/
    }

    void Tilegame::update(const double deltaTime)
    {
        scene->update(deltaTime);
        //player->update(deltaTime);

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

        scene->draw();

        //spriteBatch->begin(player->getCamera().getTransform(), true);
        //playerCharacter->draw(*spriteBatch);
        //spriteBatch->end();

        frames++;
    }
} // namespace tilegame
