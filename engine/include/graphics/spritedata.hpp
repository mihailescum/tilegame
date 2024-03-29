#pragma once

#include "glad/glad.h"

#include "core/rectangle.hpp"
#include "core/color.hpp"
#include "graphics/vertexpositiontexturecolor.hpp"

namespace engine::graphics
{
    template <class T = engine::Texture2D>
    struct SpriteData
    {
        typename T::native_type native_data;
        VertexPositionTextureColor top_left;
        VertexPositionTextureColor top_right;
        VertexPositionTextureColor bottom_left;
        VertexPositionTextureColor bottom_right;

        SpriteData() {}
        void set(
            const T &texture_data,
            const Rectangle &destination_rectangle,
            const Rectangle *const source_rectangle,
            const Color &color,
            float z)
        {
            native_data = texture_data;

            top_left.position.x = destination_rectangle.position.x;
            top_left.position.y = destination_rectangle.position.y;
            top_left.position.z = z;
            top_left.uv.x = (source_rectangle) ? source_rectangle->position.x / texture_data.dimensions().x : 0.0;
            top_left.uv.y = (source_rectangle) ? source_rectangle->position.y / texture_data.dimensions().y : 0.0;
            top_left.color = color;

            top_right.position.x = destination_rectangle.position.x + destination_rectangle.dimensions.x;
            top_right.position.y = destination_rectangle.position.y;
            top_right.position.z = z;
            top_right.uv.x = (source_rectangle) ? (source_rectangle->position.x + source_rectangle->dimensions.x) / texture_data.dimensions().x : 1.0;
            top_right.uv.y = top_left.uv.y;
            top_right.color = top_left.color;

            bottom_left.position.x = destination_rectangle.position.x;
            bottom_left.position.y = destination_rectangle.position.y + destination_rectangle.dimensions.y;
            bottom_left.position.z = z;
            bottom_left.uv.x = top_left.uv.x;
            bottom_left.uv.y = (source_rectangle) ? (source_rectangle->position.y + source_rectangle->dimensions.y) / texture_data.dimensions().y : 1.0;
            bottom_left.color = top_left.color;

            bottom_right.position.x = destination_rectangle.position.x + destination_rectangle.dimensions.x;
            bottom_right.position.y = destination_rectangle.position.y + destination_rectangle.dimensions.y;
            bottom_right.position.z = z;
            bottom_right.uv.x = top_right.uv.x;
            bottom_right.uv.y = bottom_left.uv.y;
            bottom_right.color = top_left.color;
        }
    };
} // namespace engine::graphics