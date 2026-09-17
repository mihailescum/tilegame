#pragma once

#include <variant>

#include "core/circle.hpp"
#include "core/point.hpp"
#include "core/rectangle.hpp"

namespace engine
{
    /**
     * @brief The closed set of concrete Shape kinds ECS components store by value. Components
     * must not own anything (in particular no owning pointers), so where code used to hold a
     * polymorphic `std::unique_ptr<Shape>` and `dynamic_cast<const T*>(ptr)` to inspect it, it
     * now holds a plain ShapeVariant and uses `std::get_if<T>(&variant)` instead - same
     * "try as T" idiom, no heap allocation, trivially copyable.
     */
    using ShapeVariant = std::variant<Rectangle, Circle, Point>;
} // namespace engine
