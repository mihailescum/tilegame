#pragma once

#include <glm/glm.hpp>

#include "engine.hpp"

namespace tilegame::components
{
    struct Camera
    {
        float scale;
        glm::mat4 transform;
        engine::Viewport viewport;
    };
} // namespace tilegame
