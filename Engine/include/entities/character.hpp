#ifndef __ENGINE_CHARACTER_H__
#define __ENGINE_CHARACTER_H__

#include <memory>
#include <string>
#include <cstdarg>

#include "core/resource.hpp"

#include "entities/entity.hpp"

#include "sprites/sprite.hpp"

namespace engine
{
    class Character : public Entity, public Resource
    {
    private:
        int id;
        std::unique_ptr<Sprite> sprite;

    public:
        virtual bool loadResource(ResourceManager &resourceManager, const std::string &filename, va_list args) override;
        virtual void unloadResource() override {}
    };
} // namespace engine

#endif