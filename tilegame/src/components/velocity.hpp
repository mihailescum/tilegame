#pragma once

#include <string>

#include <glm/glm.hpp>

#include "sol/sol.hpp"

namespace tilegame::components
{
    struct Velocity
    {
        float velocity;

        Velocity() : Velocity(0.0) {}
        Velocity(float velocity) : velocity(velocity) {}

        float operator()() const { return velocity; }
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame
