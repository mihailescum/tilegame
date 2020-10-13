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

        engine::Map *map1 = resourceManager.loadResource<engine::Map>("map1", "content/world/map1.tmx", this);
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
            int id = characterData.value("id", -1);
            std::string characterSource = characterData.value("file", "");

            std::filesystem::path characterPath = std::filesystem::canonical(charactersPath.parent_path() / characterSource);
            engine::Character *character = resourceManager.loadResource<engine::Character>("", characterPath, 0);
            if (character)
            {
                this->createCharacterEntity(*character);
            }
        }
    }

    void ContentSystem::createCharacterEntity(engine::Character &character) const
    {
        engine::Entity entity = scene.createEntity();
        engine::SpriteComponent &spriteComponent = entity.add<engine::SpriteComponent>(character.getSpriteSheet()->getTexture(), engine::Rectangle());
        engine::SpriteSheetComponent &spriteSheetComponent = entity.add<engine::SpriteSheetComponent>(character.getSpriteSheet(), 0, "LEFT", 0);
        engine::RenderComponent &renderComponent = entity.add<engine::RenderComponent>(character.getSpriteSheet()->getFrameWidth(), character.getSpriteSheet()->getFrameHeight(), 1.0);

        const engine::SpriteInfo *spriteInfo = character.getSpriteInfo();
        spriteComponent.sourceRectangle = spriteInfo->spriteStates.at(spriteSheetComponent.currentState)[spriteSheetComponent.currentFrame];
    }
} // namespace tilegame::worldscene