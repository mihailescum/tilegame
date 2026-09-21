#pragma once

namespace tilegame::components
{
    /**
     * @brief Keeps depth math well-conditioned regardless of how far into the map the camera has
     * travelled. `y` is a coarse world-space Y - see CHUNK_SIZE - recomputed every frame by
     * systems::Camera from this same entity's position (see components::Camera), which
     * systems::Render subtracts from an entity's own world Y before scaling it into that entity's
     * components::Depth bucket, so the subtraction result (and the float precision it needs)
     * stays small no matter how large the map or how far from world-zero play has drifted.
     *
     * Lives on the camera entity, alongside Camera itself, and is derived fresh every frame
     * rather than cached anywhere else - there is nothing to keep in sync or invalidate, so it
     * automatically follows wherever the camera currently looks (e.g. during a cutscene) rather
     * than staying anchored to the player. Not exposed to Lua - purely internal, computed
     * bookkeeping, like components::Camera itself.
     */
    struct DepthOrigin
    {
        // Large enough to comfortably exceed anything a camera could show at once (at any
        // realistic zoom), so two entities on either side of a snap boundary are never both
        // visible at once - see the discussion that ruled out a naive wraparound for this same
        // reason. Not related to map size at all - `y` snapping to a new multiple of this simply
        // keeps `y` close to the camera, however far into the map the camera has travelled.
        static constexpr float CHUNK_SIZE = 65536.0f;

        float y;
    };
} // namespace tilegame::components
