#include "tilegame.hpp"

#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <glm/gtx/transform.hpp>

#include "engine.hpp"

namespace tilegame
{
    std::unique_ptr<engine::Scene> scene;
    std::unique_ptr<engine::Observer> cameraObserver;

    engine::Map *map1;
    std::unique_ptr<engine::Renderer> map1Renderer;

    void Tilegame::initialize()
    {
        Game::initialize();

        //entity = std::make_unique<engine::FreeEntity>();
        //player = std::make_unique<engine::Player>(entity.get(), this->graphicsDevice->getViewport());

        //glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        window->setPosition(700, 400);
    }

    void Tilegame::loadContent()
    {
        Game::loadContent();

        scene = std::make_unique<engine::Scene>();
        cameraObserver = std::make_unique<engine::Observer>(scene->getRegistry());
        cameraObserver->connectOnUpdate<engine::PositionComponent, engine::CameraComponent>();

        engine::Entity cameraEntity = scene->createEntity();
        cameraEntity.add<engine::ChildComponent>();
        cameraEntity.add<engine::PositionComponent>();
        engine::CameraComponent &cameraComponent = cameraEntity.add<engine::CameraComponent>();
        cameraComponent.viewport = &this->graphicsDevice->getViewport();

        scene->getRegistry().patch<engine::PositionComponent>(cameraEntity, [](auto &pos) { pos.position = glm::vec2(0.0); });

        map1 = this->resourceManager->loadResource<engine::Map>("map1", "content/world/map1.tmx", "content/world/");
        map1Renderer = std::make_unique<engine::Renderer>(*map1);
        map1Renderer->initialize();

        //playerCharacter = this->resourceManager->loadResource<engine::Character>("playerCharacter", "content/characters/player.chr");
    }

    void Tilegame::unloadContent()
    {
        //map1Renderer.reset();
    }

    void Tilegame::processInput()
    {
        /*if (window->isKeyPressed(GLFW_KEY_LEFT))
            player->moveLeft();
        if (window->isKeyPressed(GLFW_KEY_RIGHT))
            player->moveRight();
        if (window->isKeyPressed(GLFW_KEY_UP))
            player->moveUp();
        if (window->isKeyPressed(GLFW_KEY_DOWN))
            player->moveDown();*/
    }

    void Tilegame::updateCameras()
    {
        for (auto e : *cameraObserver)
        {
            engine::CameraComponent &camera = scene->getComponent<engine::CameraComponent>(e);
            if (camera.viewport)
            {
                engine::PositionComponent &pos = scene->getComponent<engine::PositionComponent>(e);

                float scale = 1.0;
                glm::mat4 transform = glm::scale(glm::mat4(1.0), glm::vec3(scale));

                // translation - Inverting coorinates because translation matrix has to 'move' the objects into the different direction
                // translation is clipped to integers
                glm::vec3 translation(
                    floor(-(pos.position.x - camera.viewport->width / 2) * scale) / scale,
                    floor(-(pos.position.y - camera.viewport->height / 2) * scale) / scale,
                    0.0);
                transform = glm::translate(transform, translation);
                camera.transform = transform;
            }
        }
        cameraObserver->clear();

        /*cameraObserver->each(
            [](auto entity, engine::CameraComponent &camera, engine::PositionComponent &pos) {
                if (camera.viewport)
                {
                    float scale = 1.0;
                    glm::mat4 transform = glm::scale(glm::mat4(1.0), glm::vec3(scale));

                    // translation - Inverting coorinates because translation matrix has to 'move' the objects into the different direction
                    // translation is clipped to integers
                    glm::vec3 translation(
                        floor(-(pos.position.x - camera.viewport->width / 2) * scale) / scale,
                        floor(-(pos.position.y - camera.viewport->height / 2) * scale) / scale,
                        0.0);
                    transform = glm::translate(transform, translation);
                    camera.transform = transform;
                }
            });*/
    }

    void Tilegame::update(const double deltaTime)
    {
        //player->update(deltaTime);
        this->updateCameras();

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
        map1Renderer->draw(*spriteBatch, glm::mat4(1.0));

        //spriteBatch->begin(player->getCamera().getTransform(), true);
        //playerCharacter->draw(*spriteBatch);
        //spriteBatch->end();

        frames++;
    }
} // namespace tilegame
