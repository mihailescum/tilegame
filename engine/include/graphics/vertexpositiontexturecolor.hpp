#pragma once

#include <glm/glm.hpp>

namespace engine::graphics
{
    struct VertexPositionTextureColor
    {
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec4 color;
    };
} // namespace engine::graphics
