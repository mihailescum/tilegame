#pragma once

#include <glm/glm.hpp>

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Defines a view into the world. The Camera system derives `transform` each frame from
     * this entity's Transform position, `scale`, and `viewport` so it can be handed to the
     * spritebatch for rendering; it is typically pinned to the player entity via a Pin component.
     */
    struct Camera
    {
        float scale;
        /// View-projection matrix recomputed each frame from position, scale and viewport.
        glm::mat4 transform;
        engine::Viewport viewport;
    };
} // namespace tilegame
