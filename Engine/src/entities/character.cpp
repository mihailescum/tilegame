#include "entities/character.hpp"

#include <string>
#include <filesystem>
#include <tinyxml2.h>

#include "core/resourcemanager.hpp"
#include "core/rectangle.hpp"

#include "sprites/spritesheet.hpp"

namespace engine
{
    bool Character::loadResource(ResourceManager &resourceManager, va_list args)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(this->resourcePath.c_str());

        const tinyxml2::XMLElement *root = doc.FirstChildElement();
        int id = root->IntAttribute("id");

        const tinyxml2::XMLElement *propertiesElement = root->FirstChildElement("properties");
        this->parseProperties(resourceManager, propertiesElement);

        return true;
    }

    void Character::parseProperties(ResourceManager &resourceManager, const tinyxml2::XMLElement *propertiesElement)
    {
        int spriteId;
        SpriteSheet *spriteSheet;

        const tinyxml2::XMLElement *property = propertiesElement->FirstChildElement();
        while (property)
        {
            std::string name = property->Attribute("name");
            if (name == "sprite_id")
                spriteId = property->IntAttribute("value");
            else if (name == "sprite_sheet")
            {
                std::string spriteSheetSource = property->Attribute("value");
                std::filesystem::path spriteSheetPath = std::filesystem::canonical(this->resourcePath.parent_path() / spriteSheetSource);
                spriteSheet = resourceManager.loadResource<SpriteSheet>("", spriteSheetPath);
            }

            property = property->NextSiblingElement();
        }

        this->sprite = spriteSheet->getSprite(spriteId);
    }

    void Character::draw(SpriteBatch &spriteBatch) const
    {
        Rectangle destinationRect(0, 0, 16, 16);
        const Rectangle &sourceRect = this->sprite->getCurrentSourceRect();

        spriteBatch.draw(sprite->getTexture(), destinationRect, &sourceRect, Color::White);
    }
} // namespace engine