#pragma once

#include <memory>

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Geometric bounds attached to an entity for non-collision purposes, e.g. the spawn
     * region a ParticleEmitter draws random positions from. Distinct from Collider, which drives
     * physical collision resolution.
     */
    struct Shape
    {
        std::unique_ptr<engine::Shape> shape;

        Shape() = default;
        Shape(std::unique_ptr<engine::Shape> shape) : shape(std::move(shape)) {}

        const engine::Shape *operator()() const { return shape.get(); }
        engine::Shape *operator()() { return shape.get(); }
    };
} // namespace tilegame::components
