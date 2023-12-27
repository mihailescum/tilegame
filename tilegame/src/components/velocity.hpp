#pragma once

#include <glm/glm.hpp>

namespace tilegame::components
{
    struct Velocity
    {
        float velocity;

        Velocity() : Velocity(0.0) {}
        Velocity(float velocity) : velocity(velocity) {}

        float operator()() const { return velocity; }
    };
} // namespace tilegame
