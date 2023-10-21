#include "worldscene/contentsystem.hpp"

#include <fstream>
#include <string>
#include "nlohmann/json.hpp"

#include "engine.hpp"

#include "worldscene/worldscene.hpp"

namespace tilegame::worldscene
{
    ContentSystem::ContentSystem(WorldScene &scene, engine::ResourceManager &_resource_manager) : scene(scene), registry(scene.getRegistry()), _resource_manager(_resource_manager) {}

    void ContentSystem::initialize() {}

    void ContentSystem::load_content() const
    {
        this->loadCharacters();

        engine::Map *map1 = _resource_manager.load_resource<engine::Map>("map1", "content/world/map1.json", this);
        this->createMapEntity(*map1);
    }

    void ContentSystem::unload_content() const
    {
    }

    void ContentSystem::createMapEntity(engine::Map &map) const
    {
        const std::vector<std::pair<const engine::Tileset *, const int>> &tilesets = map.getTilesets();

        engine::TilesetComponent tilesetComponent(tilesets.at(0).second, tilesets.at(0).first);

        double q = 0.95;
        for (const auto &layer : map.getLayers())
        {
            engine::Entity entity = scene.createEntity();
            entity.add<engine::TileLayerComponent>(layer->getData(), map.getWidth(), map.get_height());
            entity.add<engine::TilesetComponent>(tilesetComponent);
            entity.add<engine::PositionComponent>(glm::dvec2(0.0));
            entity.add<engine::RenderComponent>(map.getWidth() * map.getTileWidth(), map.get_height() * map.getTileHeight(), 1 - q);
            if (layer->isVisible())
                entity.add<engine::VisibilityComponent>();
            if (!layer->getObjectId().empty())
                entity.addTag(layer->getObjectId());

            q *= 0.95;
        }

        for (const auto &object : map.getObjects())
        {
            if (const engine::NpcObject *npc = dynamic_cast<const engine::NpcObject *>(object.get()))
            {
                scene.E_setPosition(npc->objectId, npc->x, npc->y);
                scene.E_show(npc->objectId, true);
            }
        }
    }

    void ContentSystem::loadCharacters() const
    {
        std::filesystem::path charactersPath = "content/characters/all.json";
        nlohmann::json data;

        {
            std::ifstream fileStream(charactersPath, std::ifstream::in);
            if (!fileStream.is_open())
            {
                engine::Log::e("File could not be loaded (file: ", charactersPath, ").");
                return;
            }
            else
            {
                fileStream >> data;
            }
        }

        for (auto characterData : data.at("characters"))
        {
            std::string characterSource = characterData.value("file", "");
            std::string characterId;
            if (characterData.at("id").type() == nlohmann::json::value_t::string)
            {
                characterId = characterData.value("id", "");
            }
            else if (characterData.at("id").type() == nlohmann::json::value_t::number_integer || characterData.at("id").type() == nlohmann::json::value_t::number_unsigned)
            {
                int intId = characterData.value("id", -1);
                characterId = (intId == -1) ? "" : std::to_string(intId);
            }
            else
            {
                engine::Log::e("Id of character '", characterSource, "'must be string or unsigned int (file: ", charactersPath, ").");
                continue;
            }

            std::filesystem::path characterPath = std::filesystem::canonical(charactersPath.parent_path() / characterSource);
            engine::Character *character = _resource_manager.load_resource<engine::Character>("", characterPath, characterId.c_str());
            if (character)
            {
                engine::Entity entity = this->createCharacterEntity(*character);

                // TODO move this to startup script

                // If the entity is the player, we assign the necessary components
                if (character->getObjectId() == "0")
                {
                    entity.add<engine::InputComponent>(std::vector<int>{GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN});
                    entity.add<engine::MoveComponent>(engine::MoveComponent::MoveDirection::None, 128.0);
                    entity.add<engine::PositionComponent>();
                    engine::CameraComponent &cameraComponent = entity.add<engine::CameraComponent>();
                    entity.add<engine::VisibilityComponent>();

                    cameraComponent.viewport = &scene.get_game().get_graphics_device()->getViewport();
                    scene.E_setPosition(character->getObjectId(), 128, 128);
                }
                else if (character->getObjectId() == "1") {
                    entity.add<engine::MoveComponent>();
                }
            }
        }
    }

    engine::Entity ContentSystem::createCharacterEntity(engine::Character &character) const
    {
        engine::Entity entity = scene.createEntity();
        entity.add<engine::PositionComponent>();

        entity.add<engine::SpriteComponent>();
        registry.patch<engine::SpriteComponent>(
            entity,
            [=](auto &spriteComponent) {
                spriteComponent.texture = character.getSpriteSheet()->getTexture();
            });

        entity.add<engine::SpriteInfoComponent>();
        registry.patch<engine::SpriteInfoComponent>(
            entity,
            [=](auto &spriteInfoComponent) {
                spriteInfoComponent.spriteInfo = character.getSpriteInfo();
                spriteInfoComponent.currentState = "FRONT";
                spriteInfoComponent.currentFrame = 0;
            });

        entity.add<engine::RenderComponent>();
        registry.patch<engine::RenderComponent>(
            entity,
            [=](auto &renderComponent) {
                renderComponent.z = 1.0;
                renderComponent.width = character.getSpriteSheet()->getFrameWidth();
                renderComponent.height = character.getSpriteSheet()->getFrameHeight();
            });

        entity.add<engine::AnimationComponent>();
        registry.patch<engine::AnimationComponent>(
            entity,
            [=](auto &animationComponent) {
                animationComponent.tickDuration = 0.5;
            });

        entity.add<engine::CollisionComponent>();

        entity.add<engine::BoundingBoxComponent>();
        registry.patch<engine::BoundingBoxComponent>(
            entity,
            [=](auto &boundingBoxComponent) {
                boundingBoxComponent.boundingBox = engine::Rectangle(0, 0, 32, 32);//engine::Rectangle(1, 16, 30, 16);
            });

        if (!character.getObjectId().empty())
        {
            entity.addTag(character.getObjectId());
        }

        return entity;
    }
} // namespace tilegame::worldscene