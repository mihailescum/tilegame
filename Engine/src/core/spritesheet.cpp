#include "core/spritesheet.hpp"

#include <filesystem>
#include <string>
#include <tinyxml2.h>

namespace engine
{
    bool SpriteSheet::loadResource(ResourceManager &resourceManager, va_list args)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(this->resourcePath.c_str());

        const tinyxml2::XMLElement *root = doc.FirstChildElement();
        this->frameWidth = root->IntAttribute("tilewidth");
        this->frameHeight = root->IntAttribute("tileheight");
        this->frameCount = root->UnsignedAttribute("tilecount");
        this->columns = root->UnsignedAttribute("columns");
        this->rows = this->frameCount / this->columns;

        // Load Image
        const tinyxml2::XMLElement *image = root->FirstChildElement("image");
        std::string imageSource = image->Attribute("source");
        std::filesystem::path imagePath = std::filesystem::canonical(this->resourcePath.parent_path() / imageSource);
        this->texture = resourceManager.loadResource<Texture2D>(this->resourceName + "texture", imagePath);

        // Each sprite is represented by one terrain type
        const tinyxml2::XMLElement *spritesElement = root->FirstChildElement("terraintypes");
        this->createSprites(spritesElement);
        this->createSpriteInformation(root);

        return true;
    }

    void SpriteSheet::createSprites(const tinyxml2::XMLElement *spritesElement)
    {
        const tinyxml2::XMLElement *spriteElement = spritesElement->FirstChildElement("terrain");
        while (spriteElement)
        {
            const int defaultState = spriteElement->IntAttribute("tile");

            const tinyxml2::XMLElement *propertiesElement = spriteElement->FirstChildElement("properties");
            const tinyxml2::XMLElement *propertyElement = propertiesElement->FirstChildElement();
            int id;
            while (propertyElement)
            {
                std::string propertyName = propertyElement->Attribute("name");
                if (propertyName == "id")
                    id = propertyElement->IntAttribute("value");

                propertyElement = propertyElement->NextSiblingElement();
            }
            std::unique_ptr<SpriteInfo> sprite = std::make_unique<SpriteInfo>(id);
            this->sprites.emplace(id, std::move(sprite));

            spriteElement = spriteElement->NextSiblingElement();
        }
    }

    void SpriteSheet::createSpriteInformation(const tinyxml2::XMLElement *rootElement)
    {
        const tinyxml2::XMLElement *spriteInfoElement = rootElement->FirstChildElement("tile");
        while (spriteInfoElement)
        {
            const int frameId = spriteInfoElement->IntAttribute("id");
            const char *spriteIdRaw = spriteInfoElement->Attribute("terrain");
            if (!spriteIdRaw)
                continue;

            std::string spriteIdString(spriteIdRaw);
            spriteIdString = spriteIdString.substr(0, spriteIdString.find(','));
            if (!spriteIdString.empty())
            {
                const int spriteId = std::stoi(spriteIdString);

                const tinyxml2::XMLElement *propertiesElement = spriteInfoElement->FirstChildElement("properties");
                const tinyxml2::XMLElement *propertyElement = propertiesElement->FirstChildElement();

                std::string state;
                while (propertyElement)
                {
                    const std::string propertyName = propertyElement->Attribute("name");
                    if (propertyName == "state")
                        state = propertyElement->Attribute("value");

                    propertyElement = propertyElement->NextSiblingElement();
                }

                Rectangle frameSourceRect(
                    (frameId % this->columns) * this->frameWidth,
                    (frameId / this->columns) * this->frameHeight,
                    this->frameWidth,
                    this->frameHeight);
                this->sprites[spriteId]->addSpriteState(state, frameSourceRect);
            }

            spriteInfoElement = spriteInfoElement->NextSiblingElement();
        }
    }

    const Texture2D *SpriteSheet::getTexture() const { return this->texture; }
    const int SpriteSheet::getFrameWidth() const { return this->frameWidth; }
    const int SpriteSheet::getFrameHeight() const { return this->frameHeight; }

    const SpriteInfo &SpriteSheet::getSpriteInfo(const int spriteId) const {
        return *this->sprites.at(spriteId).get();
    }
} // namespace engine