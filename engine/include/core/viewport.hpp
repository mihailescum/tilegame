#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "rectangle.hpp"

namespace engine
{
    struct Viewport
    {
    public:
        Viewport() : Viewport(glm::ivec2(0), glm::ivec2(0)) {}
        Viewport(const Rectangle &rect) : Viewport(rect.position, rect.dimensions) {}
        Viewport(const glm::ivec2 &position, const glm::ivec2 &dimensions) : position(position), dimensions(dimensions) {}

        glm::ivec2 position;
        glm::ivec2 dimensions;
    };
} // namespace engine