#pragma once

#include <memory>

#include "engine.hpp"

namespace tilegame::components
{
    struct Collider
    {
        std::unique_ptr<engine::Shape> shape;

        Collider() = default;
        Collider(std::unique_ptr<engine::Shape> shape) : shape(std::move(shape)) {}
    };
} // namespace tilegame::components
