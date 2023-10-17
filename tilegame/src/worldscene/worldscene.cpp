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

        this->animationSystem = std::make_unique<AnimationSystem>(*this);
        this->animationSystem->initialize();

        this->spriteSystem = std::make_unique<SpriteSystem>(*this);
        this->spriteSystem->initialize();
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
        animationSystem->update(deltaTime);
        spriteSystem->update();

        movementSystem->update(deltaTime);
        cameraSystem->update(deltaTime);
    }

    void WorldScene::draw()
    {
        renderSystem->draw();
    }

    engine::Entity *WorldScene::findByObjectId(const std::string &objectId)
    {
        engine::Entity *result = nullptr;
        if (this->taggedEntities.count(objectId) == 0)
        {
            auto tagged = registry.view<engine::TagComponent>();
            for (const auto &entity : tagged)
            {
                const auto tag = tagged.get(entity);
                if (tag.tag == objectId)
                {
                    this->taggedEntities.emplace(objectId, engine::Entity(entity, this));
                    result = &this->taggedEntities[objectId];
                    break;
                }
            }
        }
        else
        {
            result = &this->taggedEntities[objectId];
        }
        return result;
    }

    void WorldScene::E_show(const std::string &objectId, const bool show)
    {
        engine::Entity *entity = findByObjectId(objectId);
        if (!entity) {
            engine::Log::w("No entity with object_id ", objectId, " exists");
            return;
        }

        if (show)
        {
            entity->add<engine::VisibilityComponent>();
        }
        else
        {
            entity->remove<engine::VisibilityComponent>();
        }
    }

    void WorldScene::E_setPosition(const std::string &objectId, const double x, const double y)
    {
        engine::Entity *entity = findByObjectId(objectId);
        if (!entity) {
            engine::Log::w("No entity with object_id ", objectId, " exists");
            return;
        }

        movementSystem->setPosition(*entity, x, y);
    }
} // namespace tilegame::worldscene