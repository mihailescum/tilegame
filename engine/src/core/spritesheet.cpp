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

        std::ifstream fileStream(this->_resource_path, std::ifstream::in);
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

    bool SpriteSheet::load_resource(ResourceManager &_resource_manager, va_list args)
    {
        std::unique_ptr<nlohmann::json> jsonDocument = this->loadJsonDocument();
        if (!jsonDocument)
        {
            Log::e("File could not be loaded (file: ", this->_resource_path, ").");
            return false;
        }

        this->frameWidth = jsonDocument->value("tilewidth", 0);
        if (this->frameWidth <= 0)
        {
            Log::e("Tile width (", this->frameWidth, ") has to be greater than zero (file: ", this->_resource_path, ").");
            return false;
        }

        this->frameHeight = jsonDocument->value("tileheight", 0);
        if (this->frameHeight <= 0)
        {
            Log::e("Tile height (", this->frameHeight, ") has to be greater than zero (file: ", this->_resource_path, ").");
            return false;
        }

        this->frameCount = jsonDocument->value("tilecount", 0);
        if (this->frameCount <= 0)
        {
            Log::e("Tile count (", this->frameCount, ") has to be greater than zero (file: ", this->_resource_path, ").");
            return false;
        }

        this->columns = jsonDocument->value("columns", 0);
        if (this->columns <= 0)
        {
            Log::e("Number of columns (", this->columns, ") has to be greater than zero (file: ", this->_resource_path, ").");
            return false;
        }

        this->rows = this->frameCount / this->columns;
        ;
        std::string imageSource = jsonDocument->value("image", "");
        if (imageSource.empty())
        {
            Log::e("Image (", imageSource, ") has to be a valid path (file: ", this->_resource_path, ").");
            return false;
        }
        auto imagePath = std::filesystem::canonical(this->_resource_path.parent_path() / imageSource);
        auto texture_ptr = _resource_manager.load_resource<Texture2D>(this->_resource_name + "texture", imagePath);
        if (texture_ptr)
            this->texture = *texture_ptr;
        else
            throw "Texture not found";

        // Each sprite is represented by one terrain type
        const nlohmann::json &spritesDocument = jsonDocument->at("terrains");
        const std::vector<int> spriteInfos = this->createSprites(spritesDocument);
        const nlohmann::json &spriteInformationsDocument = jsonDocument->at("tiles");
        this->createSpriteInformation(spriteInformationsDocument, spriteInfos);

        return true;
    }

    const std::vector<int> SpriteSheet::createSprites(const nlohmann::json &spritesDocument)
    {
        std::vector<int> result;
        for (const nlohmann::json &spriteDocument : spritesDocument)
        {
            const int defaultState = spriteDocument.at("tile");

            const nlohmann::json &propertiesDocument = spriteDocument.value("properties", nlohmann::json::array());
            if (propertiesDocument.empty())
            {
                Log::w("Sprite represented by tile ", defaultState, "has no id declared. It is skipped.");
                continue;
            }
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
            result.push_back(id);
        }
        return result;
    }

    void SpriteSheet::createSpriteInformation(const nlohmann::json &spriteInformationsDocument, const std::vector<int> &spriteIds)
    {
        /*for (const nlohmann::json &spriteInformationDocument : spriteInformationsDocument)
        {
            const int frameId = spriteInformationDocument.at("id");
            const std::vector<int> terrain = spriteInformationDocument.value<std::vector<int>>("terrain", {});
            int terrainId = -1;
            for (int id : terrain)
            {
                if (id >= 0)
                {
                    terrainId = id;
                    break;
                }
            }
            if (terrainId < 0)
                continue;

            int spriteId = spriteIds[terrainId];

            const nlohmann::json &propertiesDocument = spriteInformationDocument.value("properties", nlohmann::json::array());
            if (propertiesDocument.empty())
            {
                Log::w("Frame ", frameId, "is part of the sprite with id ", spriteId, ", but it has no stated attached. It is skipped.");
                continue;
            }

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

            if (this->sprites.count(spriteId) == 0)
            {
                this->sprites.emplace(spriteId, SpriteInfo(spriteId));
            }

            Rectangle frameSourceRect(
                (frameId % this->columns) * this->frameWidth,
                (frameId / this->columns) * this->frameHeight,
                this->frameWidth,
                this->frameHeight);

            this->sprites[spriteId].addSpriteState(state, frameSourceRect);
        }*/
    }

    const SpriteInfo *SpriteSheet::getSpriteInfo(const int spriteId) const
    {
        if (this->sprites.count(spriteId))
        {
            return &this->sprites.at(spriteId);
        }
        else
        {
            return nullptr;
        }
    }
} // namespace engine