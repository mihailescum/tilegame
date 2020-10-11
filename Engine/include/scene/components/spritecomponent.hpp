#pragma once

#include "core/texture2D.hpp"
#include "core/rectangle.hpp"

namespace engine
{
    struct SpriteComponent
    {
    public:
        SpriteComponent() = default;
        SpriteComponent(const SpriteComponent &spriteSheetComponent) = default;
        SpriteComponent(const Texture2D *texture, const Rectangle &sourceRectangle);

        const Texture2D *texture;
        Rectangle sourceRectangle;
    };
} // namespace engine