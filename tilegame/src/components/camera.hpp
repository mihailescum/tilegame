#pragma once

#include <glm/glm.hpp>

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Defines a view into the world. The Camera system derives `transform` and
     * `visible_bounds` each frame from this entity's Transform position, `scale`, and `viewport` so
     * it can be handed to the spritebatch for rendering; it is typically pinned to the player entity
     * via a Pin component.
     */
    struct Camera
    {
        float scale;
        /// View-projection matrix recomputed each frame from position, scale and viewport.
        glm::mat4 transform;
        engine::Viewport viewport;
        /// World-space rectangle currently visible through this camera, recomputed alongside
        /// `transform`. Used by Render to cull tiles/particles that fall outside the view.
        engine::Rectangle visible_bounds;
    };
} // namespace tilegame
