#pragma once

#include <string>
#include <cstdarg>
#include <unordered_map>
#include <tinyxml2.h>

#include "core/resource.hpp"
#include "core/resourcemanager.hpp"
#include "core/texture2D.hpp"
#include "core/spriteinfo.hpp"

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

        std::unordered_map<int, std::unique_ptr<SpriteInfo>> sprites;

        void createSprites(const tinyxml2::XMLElement *spritesElement);
        void createSpriteInformation(const tinyxml2::XMLElement *rootElement);

    public:
        SpriteSheet() {}
        SpriteSheet(const SpriteSheet &spriteSheet) = delete;
        ~SpriteSheet() {}

        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override {}

        const Texture2D *getTexture() const;
        const int getFrameWidth() const;
        const int getFrameHeight() const;

        const SpriteInfo &getSpriteInfo(const int spriteId) const;
    };
} // namespace engine
