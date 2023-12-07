#pragma once

#include "glad/glad.h"

#include "core/rectangle.hpp"
#include "core/color.hpp"

namespace engine::graphics
{
    struct SpriteData
    {
        engine::Rectangle destination_rectangle;
        engine::Rectangle source_rectangle;
        engine::Color color;
        GLuint gl_texture;
        float z;

        SpriteData() {}
        void set(
            const engine::Texture2D texture,
            const Rectangle &destination_rectangle,
            const Rectangle *const source_rectangle,
            const Color &color,
            float z)
        {
            this->color = color;
            this->gl_texture = texture.gl_texture();
            this->destination_rectangle = destination_rectangle;

            if (source_rectangle)
            {
                this->source_rectangle.x = source_rectangle->x / texture.width();
                this->source_rectangle.y = source_rectangle->y / texture.height();
                this->source_rectangle.width = source_rectangle->width / texture.width();
                this->source_rectangle.height = source_rectangle->height / texture.height();
            }
            else
            {
                const static engine::Rectangle default_source_rect(0.0, 0.0, 1.0, 1.0);
                this->source_rectangle = default_source_rect;
            }
            this->z = z;
        }
    };
} // namespace engine::graphics