#pragma once

#include <cstdarg>
#include <memory>
#include <string>
#include "nlohmann/json.hpp"

#include "core/resource.hpp"
#include "core/spritesheet.hpp"
#include "core/spriteinfo.hpp"

namespace engine
{
    class Character : public Resource
    {
    private:
        std::string objectId;
        const SpriteSheet *spriteSheet;
        const SpriteInfo *spriteInfo;

        const std::unique_ptr<nlohmann::json> loadJsonDocument() const;

    public:
        virtual bool load_resource(ResourceManager &_resource_manager, va_list args) override;
        virtual void unloadResource() override {}

        const std::string &getObjectId() const;
        const SpriteSheet *getSpriteSheet() const;
        const SpriteInfo *getSpriteInfo() const;
    };
} // namespace engine