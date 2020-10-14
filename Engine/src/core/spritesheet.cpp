#include "core/spritesheet.hpp"

#include <filesystem>
#include <fstream>
#include <string>

#include "core/log.hpp"

namespace engine
{
    const std::unique_ptr<nlohmann::json> SpriteSheet::loadJsonDocument() const
    {
        std::unique_ptr<nlohmann::json> result = std::make_unique<nlohmann::json>();

        std::ifstream fileStream(this->resourcePath, std::ifstream::in);
        if (!fileStream.is_open())
        {
            return nullptr;
        }
        else
        {
            fileStream >> *result;
        }
        return result;
    }

    bool SpriteSheet::loadResource(ResourceManager &resourceManager, va_list args)
    {
        std::unique_ptr<nlohmann::json> jsonDocument = this->loadJsonDocument();
        if (!jsonDocument)
        {
            Log::e("File could not be loaded (file: ", this->resourcePath, ").");
            return false;
        }

        this->frameWidth = jsonDocument->value("tilewidth", 0);
        if (this->frameWidth <= 0)
        {
            Log::e("Tile width (", this->frameWidth, ") has to be greater than zero (file: ", this->resourcePath, ").");
            return false;
        }

        this->frameHeight = jsonDocument->value("tileheight", 0);
        if (this->frameHeight <= 0)
        {
            Log::e("Tile height (", this->frameHeight, ") has to be greater than zero (file: ", this->resourcePath, ").");
            return false;
        }

        this->frameCount = jsonDocument->value("tilecount", 0);
        if (this->frameCount <= 0)
        {
            Log::e("Tile count (", this->frameCount, ") has to be greater than zero (file: ", this->resourcePath, ").");
            return false;
        }

        this->columns = jsonDocument->value("columns", 0);
        if (this->columns <= 0)
        {
            Log::e("Number of columns (", this->columns, ") has to be greater than zero (file: ", this->resourcePath, ").");
            return false;
        }

        this->rows = this->frameCount / this->columns;
        ;
        std::string imageSource = jsonDocument->value("image", "");
        if (imageSource.empty())
        {
            Log::e("Image (", imageSource, ") has to be a valid path (file: ", this->resourcePath, ").");
            return false;
        }
        std::filesystem::path imagePath = std::filesystem::canonical(this->resourcePath.parent_path() / imageSource);
        this->texture = resourceManager.loadResource<Texture2D>(this->resourceName + "texture", imagePath);

        // Each sprite is represented by one terrain type
        const nlohmann::json &spritesElement = jsonDocument->at("terrains");
        this->createSprites(spritesElement);
        const nlohmann::json &spriteInformationsElement = jsonDocument->at("tiles");
        this->createSpriteInformation(spriteInformationsElement);

        return true;
    }

    void SpriteSheet::createSprites(const nlohmann::json &spritesDocument)
    {
        for (const nlohmann::json &spriteDocument : spritesDocument)
        {
            const int defaultState = spriteDocument.at("tile");

            const nlohmann::json &propertiesDocument = spriteDocument.at("properties");
            int id;
            for (const nlohmann::json &propertyDocument : propertiesDocument)
            {
                const std::string propertyName = propertyDocument.value("name", "");
                const std::string propertyType = propertyDocument.value("type", "");
                if (propertyName == "id")
                {
                    if (propertyType == "int")
                        id = propertyDocument.value("value", -1);
                    else if (propertyType == "string")
                        id = std::stoi(propertyDocument.value("value", "-1"));
                }
            }
            std::unique_ptr<SpriteInfo> sprite = std::make_unique<SpriteInfo>(id);
            this->sprites.emplace(id, std::move(sprite));
        }
    }

    void SpriteSheet::createSpriteInformation(const nlohmann::json &spriteInformationsElement)
    {
        for (const nlohmann::json &spriteInformationDocument : spriteInformationsElement)
        {
            const int frameId = spriteInformationDocument.at("id");
            const std::vector<int> terrain = spriteInformationDocument.value<std::vector<int>>("terrain", {});
            int spriteId = -1;
            for (int id : terrain)
            {
                if (id >= 0)
                {
                    spriteId = id;
                    break;
                }
            }
            if (spriteId < 0)
                continue;

            const nlohmann::json &propertiesDocument = spriteInformationDocument.at("properties");
            std::string state;
            for (const nlohmann::json &propertyDocument : propertiesDocument)
            {
                const std::string propertyName = propertyDocument.value("name", "");
                const std::string propertyType = propertyDocument.value("type", "");
                if (propertyName == "state")
                {
                    if (propertyType == "string")
                        state = propertyDocument.value("value", "");
                }
            }

            Rectangle frameSourceRect(
                (frameId % this->columns) * this->frameWidth,
                (frameId / this->columns) * this->frameHeight,
                this->frameWidth,
                this->frameHeight);
            this->sprites[spriteId]->addSpriteState(state, frameSourceRect);
        }
    }

    const Texture2D *SpriteSheet::getTexture() const { return this->texture; }
    const int SpriteSheet::getFrameWidth() const { return this->frameWidth; }
    const int SpriteSheet::getFrameHeight() const { return this->frameHeight; }

    const SpriteInfo &SpriteSheet::getSpriteInfo(const int spriteId) const
    {
        return *this->sprites.at(spriteId).get();
    }
} // namespace engine