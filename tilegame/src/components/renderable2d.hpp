#pragma once

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Empty tag marking an entity as visible to the Render system, which draws it as a
     * Sprite, TileLayer, or ParticlePool depending on which of those components is also present.
     * Exposed to Lua as `_Renderable2D`.
     */
    struct Renderable2D
    {
        static void register_component(sol::state &lua);
    };
} // namespace tilegame
