#pragma once

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief The actual Z an entity's draw call is issued with, in the same units the
     * SpriteBatch's ortho projection covers (-1..1 - see engine::graphics::SpriteBatch::create()).
     * The GPU's depth test (see systems::Render's DepthMode::TestAndWrite pass), not any CPU
     * sort, decides per-pixel which of two overlapping opaque draws wins; whoever builds an
     * entity is responsible for placing it at a sensible Z within that range (e.g.
     * content/scripts/maploader.lua bins tile layers into small, distinct values). Exposed to Lua
     * as `_Depth`.
     */
    struct Depth
    {
        float z;

        float operator()() const { return z; }

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
