#pragma once

#include <glm/glm.hpp>

#include "engine.hpp"
#include "helper.hpp"

namespace tilegame
{
    inline glm::vec2 random_point_in_rectangle(const engine::Rectangle &rect)
    {
        float x = get_random(rect.position.x, rect.position.x + rect.dimensions.x);
        float y = get_random(rect.position.y, rect.position.y + rect.dimensions.y);
        return glm::vec2(x, y);
    }

    template <typename T>
    inline int sign(T val)
    {
        // https://stackoverflow.com/a/4609795
        // CC BY-SA 4.0 license (see https://creativecommons.org/licenses/by-sa/4.0/)
        return (T(0) < val) - (val < T(0));
    }
} // namespace tilegame
