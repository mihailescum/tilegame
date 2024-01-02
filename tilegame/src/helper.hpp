#pragma once

#include <random>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace tilegame
{
    inline float get_random()
    {
        static std::default_random_engine e;
        static std::uniform_real_distribution<> dis(0, 1); // range [0, 1)
        return dis(e);
    }

    inline float get_random(float min, float max)
    {
        float v = get_random();
        return v * (max - min) + min;
    }
} // namespace tilegame
