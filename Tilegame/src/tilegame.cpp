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
    std::unique_ptr<worldscene::WorldScene> scene;

    void Tilegame::initialize()
    {
        Game::initialize();

        this->spriteBatch = std::make_unique<engine::SpriteBatch>(*this->graphicsDevice);

        //entity = std::make_unique<engine::FreeEntity>();
        //player = std::make_unique<engine::Player>(entity.get(), this->graphicsDevice->getViewport());

        //glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        window->setPosition(700, 400);

        scene = std::make_unique<worldscene::WorldScene>(*this);
        scene->initialize();
    }

    void Tilegame::loadContent()
    {
        Game::loadContent();

        this->spriteBatch->create();
        scene->loadContent();

        //playerCharacter = this->resourceManager->loadResource<engine::Character>("playerCharacter", "content/characters/player.chr");
    }

    void Tilegame::unloadContent()
    {
        //map1Renderer.reset();
        this->spriteBatch.reset();
        scene->unloadContent();
    }

    void Tilegame::processInput()
    {
        scene->processInput();
    }

    void Tilegame::update(const double deltaTime)
    {
        scene->update(deltaTime);

        timer += deltaTime;
        updates++;
        // - Reset after one second
        if (timer > 1)
        {
            timer--;
            std::stringstream ss;
            ss << "FPS: " << frames << " - UPS: " << updates;
            //window->setTitle(ss.str());
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

    engine::SpriteBatch *Tilegame::getSpriteBatch() const {
        return this->spriteBatch.get();
    }
} // namespace tilegame
