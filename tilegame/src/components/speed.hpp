#pragma once

#include <string>

#include <glm/glm.hpp>

#include "sol/sol.hpp"

namespace tilegame::components
{
    struct Speed
    {
        float speed;

        Speed() : Speed(0.0) {}
        Speed(float speed) : speed(speed) {}

        float operator()() const { return speed; }
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame
