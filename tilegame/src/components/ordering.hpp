#pragma once

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Draw-order key for Renderable2D entities. The Render system sorts entities by `z`
     * ascending before each frame and redraws that order whenever an Ordering component is
     * constructed, updated or destroyed. Exposed to Lua as `_Ordering`.
     */
    struct Ordering
    {
        float z;

        float operator()() const { return z; }

        static void register_component(sol::state &lua);
    };
} // namespace tilegame
