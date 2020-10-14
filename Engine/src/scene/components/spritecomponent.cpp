#include "scene/components/spritecomponent.hpp"

namespace engine
{
    SpriteComponent::SpriteComponent(const Texture2D *texture, const Rectangle &sourceRectangle)
        : texture(texture), sourceRectangle(sourceRectangle)
    {
    }
} // namespace engine