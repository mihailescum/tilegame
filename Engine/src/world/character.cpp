#include "world/character.hpp"

#include <fstream>
#include <filesystem>
#include <sstream>
#include "nlohmann/json.hpp"

#include "core/log.hpp"

namespace engine
{
    const std::unique_ptr<nlohmann::json> Character::loadJsonDocument() const
    {
        std::unique_ptr<nlohmann::json> result = std::make_unique<nlohmann::json>();

        std::ifstream fileStream(this->resourcePath, std::ifstream::in);
        if (!fileStream.is_open())
        {
            std::stringstream ss;
            ss << "File could not be loaded (file: " << this->resourcePath << ")." << std::endl;
            Log::e(ss.str());
            return nullptr;
        }
        else
        {
            fileStream >> *result;
        }
        return result;
    }

    bool Character::loadResource(ResourceManager &resourceManager, va_list args)
    {
        std::unique_ptr<nlohmann::json> jsonDocument = this->loadJsonDocument();
        if (!jsonDocument)
            return false;

        this->objectId = va_arg(args, const char *);

        int spriteId = jsonDocument->value("sprite_id", 0);
        std::string spriteSheetSource = jsonDocument->value("sprite_sheet", "");
        if (spriteSheetSource.empty())
        {
            std::stringstream ss;
            ss << "'sprite_sheet' field does not exist or is empty (file: " << this->resourcePath << ")." << std::endl;
            Log::e(ss.str());
            return false;
        }

        std::filesystem::path spriteSheetPath = std::filesystem::canonical(this->resourcePath.parent_path() / spriteSheetSource);
        this->spriteSheet = resourceManager.loadResource<SpriteSheet>("", spriteSheetPath);
        this->spriteInfo = &this->spriteSheet->getSpriteInfo(spriteId);
        return true;
    }

    const std::string &Character::getObjectId() const { return this->objectId; }
    const SpriteSheet *Character::getSpriteSheet() const { return this->spriteSheet; }
    const SpriteInfo *Character::getSpriteInfo() const { return this->spriteInfo; }

} // namespace engine