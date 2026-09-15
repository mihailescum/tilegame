#pragma once

#include <memory>

#include "engine.hpp"

namespace tilegame::components
{
    /**
     * @brief Attaches a collision shape to an entity so the CollisionDetection system can sweep it
     * against TileLayer tile shapes and deflect its Movement velocity on contact.
     */
    struct Collider
    {
        std::unique_ptr<engine::Shape> shape;

        Collider() = default;
        Collider(std::unique_ptr<engine::Shape> shape) : shape(std::move(shape)) {}
    };
} // namespace tilegame::components
