#include "worldscene/worldscene.hpp"

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "engine.hpp"

#include "tilegame.hpp"
#include "worldscene/contentsystem.hpp"
#include "worldscene/inputsystem.hpp"
#include "worldscene/camerasystem.hpp"
#include "worldscene/movementsystem.hpp"
#include "worldscene/rendersystem.hpp"

namespace tilegame::worldscene
{
    engine::Entity playerEntity;

    WorldScene::WorldScene(Tilegame &game) : spriteBatch(*game.getSpriteBatch()), engine::Scene(game)
    {
    }

    void WorldScene::initialize()
    {
        this->createSystems();
    }

    void WorldScene::createSystems()
    {
        this->contentSystem = std::make_unique<ContentSystem>(*this, *game.getResourceManager());
        this->contentSystem->initialize();

        this->inputSystem = std::make_unique<InputSystem>(*this, *game.getWindow());
        this->inputSystem->initialize();

        this->cameraSystem = std::make_unique<CameraSystem>(*this);
        this->cameraSystem->initialize();

        this->movementSystem = std::make_unique<MovementSystem>(*this);
        this->movementSystem->initialize();

        this->renderSystem = std::make_unique<RenderSystem>(*this, this->spriteBatch);
        this->renderSystem->initialize();
    }

    void WorldScene::loadContent()
    {
        contentSystem->loadContent();
    }

    void WorldScene::unloadContent()
    {
        contentSystem->unloadContent();
    }

    void WorldScene::processInput()
    {
        inputSystem->processInput();
    }

    void WorldScene::update(const double deltaTime)
    {
        movementSystem->update(deltaTime);
        cameraSystem->update(deltaTime);
    }

    void WorldScene::draw()
    {
        renderSystem->draw();
    }
} // namespace tilegame::worldscene