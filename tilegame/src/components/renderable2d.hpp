#pragma once

namespace tilegame::components
{
    /**
     * @brief Empty tag marking an entity as visible to the Render system, which draws it as a
     * Sprite, TileLayer, or ParticlePool depending on which of those components is also present.
     */
    struct Renderable2D
    {
    };
} // namespace tilegame
