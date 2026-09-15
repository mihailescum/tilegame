#pragma once

namespace tilegame::components
{
    /**
     * @brief Draw-order key for Renderable2D entities. The Render system sorts entities by `z`
     * ascending before each frame and redraws that order whenever an Ordering component is
     * constructed, updated or destroyed.
     */
    struct Ordering
    {
        float z;

        float operator()() const { return z; }
    };
} // namespace tilegame
