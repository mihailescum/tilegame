#include "worldscene/contentsystem.hpp"

#include <fstream>
#include <string>
#include "nlohmann/json.hpp"

#include "engine.hpp"

namespace tilegame::worldscene
{
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
        const std::vector<std::unique_ptr<const engine::TileLayer>> &layers = map.getLayers();

        engine::TilesetComponent tilesetComponent(tilesets.at(0).second, tilesets.at(0).first);

        double q = 1;
        for (auto &&layer : layers)
        {
            engine::Entity entity = scene.createEntity();
            entity.add<engine::TileLayerComponent>(layer->getData(), map.getWidth(), map.getHeight());
            entity.add<engine::TilesetComponent>(tilesetComponent);
            entity.add<engine::PositionComponent>(glm::vec2(0.0));
            entity.add<engine::RenderComponent>(map.getWidth() * map.getTileWidth(), map.getHeight() * map.getTileHeight(), 1 - q);
            if (layer->isVisible())
                entity.add<engine::VisiblityComponent>();

            q *= 0.5;
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
                std::stringstream ss;
                ss << "File could not be loaded (file: " << charactersPath << ")." << std::endl;
                engine::Log::e(ss.str());
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
                std::stringstream ss;
                ss << "Id of character '" << characterSource << "'must be string or unsigned int (file: " << charactersPath << ")." << std::endl;
                engine::Log::e(ss.str());
                continue;
            }

            std::filesystem::path characterPath = std::filesystem::canonical(charactersPath.parent_path() / characterSource);
            engine::Character *character = resourceManager.loadResource<engine::Character>("", characterPath, characterId.c_str());
            if (character)
            {
                engine::Entity entity = this->createCharacterEntity(*character);

                // TODO move this to startup script

                // If the entity is the player, we assign the necessary components
                if (character->getId() == "0")
                {
                    entity.add<engine::InputComponent>(std::vector<int>{GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_UP, GLFW_KEY_DOWN});
                    entity.add<engine::MoveComponent>(engine::MoveComponent::MoveDirection::None, 128.0);
                    entity.add<engine::PositionComponent>();
                    engine::CameraComponent &cameraComponent = entity.add<engine::CameraComponent>();
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
        engine::SpriteSheetComponent &spriteSheetComponent = entity.add<engine::SpriteSheetComponent>(character.getSpriteSheet(), 0, "LEFT", 0);
        engine::RenderComponent &renderComponent = entity.add<engine::RenderComponent>(character.getSpriteSheet()->getFrameWidth(), character.getSpriteSheet()->getFrameHeight(), 1.0);
        entity.add<engine::VisiblityComponent>();

        const engine::SpriteInfo *spriteInfo = character.getSpriteInfo();
        spriteComponent.sourceRectangle = spriteInfo->spriteStates.at(spriteSheetComponent.currentState)[spriteSheetComponent.currentFrame];
        return entity;
    }
} // namespace tilegame::worldscene