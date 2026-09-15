#pragma once

#include <glm/glm.hpp>

namespace tilegame::components
{
    /**
     * @brief Per-frame displacement applied to an entity's Transform position by the Movement system.
     * Computed by MovementController from Direction and Speed, and clamped/deflected by
     * CollisionDetection before being applied.
     */
    struct Movement
    {
        /// Displacement for the current frame, already scaled by elapsed time (not a per-second rate).
        glm::vec2 velocity;
    };
} // namespace tilegame
