#pragma once

#include <memory>
#include <string>
#include <cstdarg>
#include <tinyxml2.h>

#include "core/resource.hpp"
#include "core/spritebatch.hpp"

#include "entities/entity.hpp"

#include "sprites/sprite.hpp"

namespace engine
{
    class Character : public Entity, public Resource
    {
    private:
        int id;
        Sprite *sprite;

        void parseProperties(ResourceManager &resourceManager, const tinyxml2::XMLElement *propertiesElement);

    public:
        virtual bool loadResource(ResourceManager &resourceManager, va_list args) override;
        virtual void unloadResource() override {}

        void draw(SpriteBatch &spriteBatch) const;
    };
} // namespace engine
