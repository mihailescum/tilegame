#pragma once

#include <random>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

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

    glm::vec2 clamp_with_precision(const glm::vec2 &x, const glm::vec2 &target, float precision)
    {
        return (glm::length2(x - target) < precision * precision) ? target : x;
    }
} // namespace tilegame
