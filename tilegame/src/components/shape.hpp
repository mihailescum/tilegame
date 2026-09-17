#pragma once

#include "sol/sol.hpp"

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Geometric bounds attached to an entity for non-collision purposes, e.g. the spawn
     * region a ParticleEmitter draws random positions from, or a map entity's pixel-space extent
     * (see systems::World). Distinct from Collider, which drives physical collision resolution.
     * A plain value (engine::ShapeVariant) - components own nothing, so this is never a pointer.
     * Exposed to Lua as `_Shape`.
     */
    struct Shape
    {
        engine::ShapeVariant shape;

        Shape() = default;
        Shape(const engine::ShapeVariant &shape) : shape(shape) {}

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
