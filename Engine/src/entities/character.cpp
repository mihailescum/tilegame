#include "entities/character.hpp"

#include <string>
#include <filesystem>
#include "tinyxml2.h"

#include "core/resourcemanager.hpp"

#include "sprites/spritesheet.hpp"

namespace engine
{
    bool Character::loadResource(ResourceManager &resourceManager, const std::string &filename, va_list args)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(filename.c_str());

        const tinyxml2::XMLElement *root = doc.FirstChildElement();
        int id = root->IntAttribute("id");

        const tinyxml2::XMLElement *properties = root->FirstChildElement("properties");
        const tinyxml2::XMLElement *property = properties->FirstChildElement();

        int spriteId;
        SpriteSheet *spriteSheet;
        while (property)
        {
            std::string name = property->Attribute("name");
            if (name == "sprite_id")
                spriteId = property->IntAttribute("value");
            else if (name == "sprite_sheet")
            {
                std::string spriteSheetSource = property->Attribute("value");
                std::filesystem::path spriteSheetPath = std::filesystem::canonical(std::filesystem::absolute(filename).parent_path() / spriteSheetSource);
                spriteSheet = resourceManager.loadResource<SpriteSheet>("", spriteSheetPath);
            }

            property = property->NextSiblingElement();
        }

        //this->sprite = spriteSheet->getSprite(spriteId);
        return true;
    }
} // namespace engine