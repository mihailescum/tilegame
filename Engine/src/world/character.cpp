#include "world/character.hpp"

#include <fstream>
#include <filesystem>
#include <sstream>
#include "nlohmann/json.hpp"

#include "core/log.hpp"

namespace engine
{
    bool Character::loadResource(ResourceManager &resourceManager, va_list args)
    {
        nlohmann::json data;

        {
            std::ifstream fileStream(this->resourcePath, std::ifstream::in);
            if (!fileStream.is_open())
            {
                std::stringstream ss;
                ss << "File could not be loaded (file: " << this->resourcePath << ")." << std::endl;
                Log::e(ss.str());
                return false;
            }
            else
            {
                fileStream >> data;
            }
        }

        this->id = va_arg(args, const char *);

        int spriteId = data.value("sprite_id", 0);
        std::string spriteSheetSource = data.value("sprite_sheet", "");
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

    const std::string &Character::getId() const { return this->id; }
    const SpriteSheet *Character::getSpriteSheet() const { return this->spriteSheet; }
    const SpriteInfo *Character::getSpriteInfo() const { return this->spriteInfo; }

} // namespace engine