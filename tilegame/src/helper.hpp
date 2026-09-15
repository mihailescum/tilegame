#pragma once

#include <random>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace tilegame
{
    // Returns a uniformly distributed float in [0, 1) from a function-local, unseeded
    // engine shared across all calls (so output is deterministic across runs but not
    // safe to call concurrently from multiple threads).
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
