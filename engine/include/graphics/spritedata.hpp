#pragma once

#include "glad/glad.h"

#include "core/rectangle.hpp"
#include "core/color.hpp"
#include "graphics/vertexpositiontexturecolor.hpp"

namespace engine::graphics
{
    struct SpriteData
    {
        GLuint gl_texture;
        VertexPositionTextureColor top_left;
        VertexPositionTextureColor top_right;
        VertexPositionTextureColor bottom_left;
        VertexPositionTextureColor bottom_right;

        SpriteData() {}
        void set(
            const engine::Texture2D texture,
            const Rectangle &destination_rectangle,
            const Rectangle *const source_rectangle,
            const Color &color,
            float z)
        {
            gl_texture = texture.gl_texture();

            top_left.position.x = destination_rectangle.x;
            top_left.position.y = destination_rectangle.y;
            top_left.position.z = z;
            top_left.uv.x = (source_rectangle) ? source_rectangle->x / texture.width() : 0.0;
            top_left.uv.y = (source_rectangle) ? source_rectangle->y / texture.height() : 0.0;
            top_left.color = color;

            top_right.position.x = destination_rectangle.x + destination_rectangle.width;
            top_right.position.y = destination_rectangle.y;
            top_right.position.z = z;
            top_right.uv.x = (source_rectangle) ? (source_rectangle->x + source_rectangle->width) / texture.width() : 1.0;
            top_right.uv.y = top_left.uv.y;
            top_right.color = color;

            bottom_left.position.x = destination_rectangle.x;
            bottom_left.position.y = destination_rectangle.y + destination_rectangle.height;
            bottom_left.position.z = z;
            bottom_left.uv.x = top_left.uv.x;
            bottom_left.uv.y = (source_rectangle) ? (source_rectangle->y + source_rectangle->height) / texture.height() : 1.0;
            bottom_left.color = color;

            bottom_right.position.x = destination_rectangle.x + destination_rectangle.width;
            bottom_right.position.y = destination_rectangle.y + destination_rectangle.height;
            bottom_right.position.z = z;
            bottom_right.uv.x = top_right.uv.x;
            bottom_right.uv.y = bottom_left.uv.y;
            bottom_right.color = color;
        }
    };
} // namespace engine::graphics