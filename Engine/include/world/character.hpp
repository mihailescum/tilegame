#pragma once

#include "core/resource.hpp"
#include "core/spritesheet.hpp"
#include "core/spriteinfo.hpp"

namespace engine
{
    class Character : public Resource
    {
    private:
        std::string id;
        const SpriteSheet *spriteSheet;
        const SpriteInfo *spriteInfo;

    public:
        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override {}

        const std::string &getId() const;
        const SpriteSheet *getSpriteSheet() const;
        const SpriteInfo *getSpriteInfo() const;
    };
} // namespace engine