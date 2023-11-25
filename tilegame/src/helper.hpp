#pragma once

#include <random>

namespace tilegame
{
    float get_random()
    {
        static std::default_random_engine e;
        static std::uniform_real_distribution<> dis(0, 1); // range [0, 1)
        return dis(e);
    }

    float get_random(float min, float max)
    {
        float v = get_random();
        return v * (max - min) + min;
    }
} // namespace tilegame
