#pragma once

#include <glm/glm.hpp>

#include "engine.hpp"
#include "helper.hpp"

namespace tilegame
{
    glm::vec2 random_point_in_rectangle(const engine::Rectangle &rect)
    {
        float x = get_random(rect.x, rect.x + rect.width);
        float y = get_random(rect.y, rect.y + rect.height);
        return glm::vec2(x, y);
    }
} // namespace tilegame
