#pragma once

#include <string>

#include <glm/glm.hpp>
#include "entt/entt.hpp"

#include "sol/sol.hpp"

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Defines a view into the world. The Camera system derives `transform` and
     * `visible_bounds` each frame from this entity's Transform position, `scale`, and `viewport` so
     * it can be handed to the spritebatch for rendering; it is typically pinned to the player entity
     * via a Pin component. Exposed to Lua as `_Camera` (`scale` and `visible_bounds` only -
     * `transform`/`viewport` have no Lua bindings of their own, and components::DepthOrigin,
     * which also lives on this entity, is intentionally not exposed at all).
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

        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };

    // Registry context id (registry.ctx()) under which the single camera entity's handle is
    // stored - created once by systems::Camera::load_content(), and read by systems::Render to
    // know what to draw through. There is only ever one camera (see systems::Camera), so this
    // is a named ctx() entry rather than a view, the same way systems::Weather tracks its
    // precipitation entity - shared between two systems here, unlike Weather's, hence living in
    // this header rather than privately in either .cpp.
    inline constexpr entt::id_type CAMERA_ENTITY_ID = entt::hashed_string("camera_entity").value();
} // namespace tilegame
