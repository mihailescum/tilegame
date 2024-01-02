#pragma once

#include <glm/glm.hpp>

#include "engine.hpp"
#include "helper.hpp"

namespace tilegame
{
    glm::vec2 random_point_in_rectangle(const engine::Rectangle &rect)
    {
        float x = get_random(rect.position.x, rect.position.x + rect.size.x);
        float y = get_random(rect.position.y, rect.position.y + rect.size.y);
        return glm::vec2(x, y);
    }
} // namespace tilegame
