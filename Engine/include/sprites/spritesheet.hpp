#pragma once

#include <string>
#include <cstdarg>
#include <unordered_map>
#include <tinyxml2.h>

#include "core/resource.hpp"
#include "core/resourcemanager.hpp"
#include "core/texture2D.hpp"

#include "sprites/sprite.hpp"

namespace engine
{
    class SpriteSheet : public Resource
    {
    private:
        Texture2D *texture;
        int frameCount;
        int frameWidth;
        int frameHeight;
        int rows;
        int columns;

        std::unordered_map<int, std::unique_ptr<Sprite>> sprites;

        void createSprites(const tinyxml2::XMLElement *spritesElement);
        void createSpriteInformation(const tinyxml2::XMLElement *rootElement);

    public:
        SpriteSheet() {}
        SpriteSheet(const SpriteSheet &spriteSheet) = delete;
        ~SpriteSheet() {}

        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override {}

        Sprite *getSprite(const int spriteId) const;
    };
} // namespace engine
