#include "worldscene/contentsystem.hpp"

#include <fstream>
#include <string>
#include "nlohmann/json.hpp"

#include "engine.hpp"

#include "worldscene/worldscene.hpp"

namespace tilegame::worldscene
{
    ContentSystem::ContentSystem(WorldScene &scene, engine::ResourceManager &resourceManager) : scene(scene), resourceManager(resourceManager) {}

    void ContentSystem::initialize() {}

    void ContentSystem::loadContent() const
    {
        this->loadCharacters();

        engine::Map *map1 = resourceManager.loadResource<engine::Map>("map1", "content/world/map1.json", this);
        this->createMapEntity(*map1);
    }

    void ContentSystem::unloadContent() const
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
            entity.add<engine::TileLayerComponent>(layer->getData(), map.getWidth(), map.getHeight());
            entity.add<engine::TilesetComponent>(tilesetComponent);
            entity.add<engine::PositionComponent>(glm::vec2(0.0));
            entity.add<engine::RenderComponent>(map.getWidth() * map.getTileWidth(), map.getHeight() * map.getTileHeight(), 1 - q);
            if (layer->isVisible())
                entity.add<engine::VisiblityComponent>();
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
            engine::Character *character = resourceManager.loadResource<engine::Character>("", characterPath, characterId.c_str());
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
                    entity.add<engine::VisiblityComponent>();

                    cameraComponent.viewport = &scene.getGame().getGraphicsDevice()->getViewport();
                    this->scene.getRegistry().patch<engine::PositionComponent>(entity, [](auto &pos) { pos.position = glm::vec2(0.0); });
                }
            }
        }
    }

    engine::Entity ContentSystem::createCharacterEntity(engine::Character &character) const
    {
        engine::Entity entity = scene.createEntity();
        engine::SpriteComponent &spriteComponent = entity.add<engine::SpriteComponent>(character.getSpriteSheet()->getTexture(), engine::Rectangle());
        engine::SpriteInfoComponent &spriteSheetComponent = entity.add<engine::SpriteInfoComponent>(character.getSpriteInfo(), "FRONT", 0);
        engine::RenderComponent &renderComponent = entity.add<engine::RenderComponent>(character.getSpriteSheet()->getFrameWidth(), character.getSpriteSheet()->getFrameHeight(), 1.0);
        engine::PositionComponent &positionComponent = entity.add<engine::PositionComponent>();

        if (!character.getObjectId().empty())
            entity.addTag(character.getObjectId());

        const engine::SpriteInfo *spriteInfo = character.getSpriteInfo();
        spriteComponent.sourceRectangle = spriteInfo->spriteStates.at(spriteSheetComponent.currentState)[spriteSheetComponent.currentFrame];
        return entity;
    }
} // namespace tilegame::worldscene