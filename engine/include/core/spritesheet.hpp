#pragma once

#include <cstdarg>
#include <memory>
#include <string>
#include <unordered_map>
#include "nlohmann/json.hpp"

#include "resource.hpp"
#include "resourcemanager.hpp"
#include "texture2D.hpp"
#include "spriteinfo.hpp"

namespace engine
{
    class SpriteSheet : public Resource
    {
    private:
        Texture2D &texture;
        int frameCount;
        int frameWidth;
        int frameHeight;
        int rows;
        int columns;

        std::unordered_map<int, SpriteInfo> sprites;

        const std::unique_ptr<nlohmann::json> loadJsonDocument() const;
        const std::vector<int> createSprites(const nlohmann::json &spritesDocument);
        void createSpriteInformation(const nlohmann::json &spriteInformationsDocument, const std::vector<int> &spriteIds);

    public:
        SpriteSheet(Texture2D &texture) : texture(texture) {}
        SpriteSheet(const SpriteSheet &spriteSheet) = delete;
        ~SpriteSheet() {}

        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override {}

        const Texture2D &getTexture() const { return texture; }
        const int getFrameWidth() const { return frameWidth; }
        const int getFrameHeight() const { return frameHeight; }

        const SpriteInfo *getSpriteInfo(const int spriteId) const;
    };
} // namespace engine
