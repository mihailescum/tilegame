#pragma once

#include <memory>

#include "engine.hpp"

namespace tilegame::components
{
    struct Shape
    {
        std::unique_ptr<engine::Shape> shape;

        const engine::Shape *operator()() const { return shape.get(); }
        engine::Shape *operator()() { return shape.get(); }
    };
} // namespace tilegame::components
