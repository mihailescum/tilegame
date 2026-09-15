#pragma once

#include <glm/glm.hpp>

namespace engine::graphics
{
    /**
     * @brief GPU vertex layout (position, UV, color) matching the vertex attributes
     * consumed by SpriteBatch's shader and VAO.
     */
    struct VertexPositionTextureColor
    {
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec4 color;
    };
} // namespace engine::graphics
