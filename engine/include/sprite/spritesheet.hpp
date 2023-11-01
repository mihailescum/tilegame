#pragma once

#include "core/resource.hpp"

namespace engine::sprite
{
    class SpriteSheet : engine::Resource
    {
    public:
        virtual bool load_resource(ResourceManager &resource_manager, va_list args) override;
        virtual void unload_resource() override;
    };
} // namespace engine::sprite
