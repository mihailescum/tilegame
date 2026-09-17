#pragma once

#include "sol/sol.hpp"

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Attaches a collision shape to an entity so the CollisionDetection system can sweep it
     * against TileLayer tile shapes and deflect its Movement velocity on contact. A plain value
     * (engine::ShapeVariant) - components own nothing, so this is never a pointer.
     */
    struct Collider
    {
        engine::ShapeVariant shape;

        Collider() = default;
        Collider(const engine::ShapeVariant &shape) : shape(shape) {}

        /// Builds a ShapeVariant from a Lua-supplied descriptor table: {kind = "rectangle",
        /// position = vec2, dimensions = vec2}, {kind = "circle", position = vec2, radius =
        /// number}, or {kind = "point", position = vec2}. Shared with TileLayer::build(), which
        /// parses per-tile collision shapes from the same descriptor shape.
        static engine::ShapeVariant make_shape(const sol::table &descriptor);
    };
} // namespace tilegame::components
