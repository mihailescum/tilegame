#pragma once

namespace engine
{
    /**
     * @brief Abstract base for 2D geometric primitives (Point, Circle, Ray, Rectangle, ...).
     *
     * Provides a common polymorphic interface, notably clone() for
     * value-copying a shape through a base-class pointer.
     */
    struct Shape
    {
        virtual ~Shape() = 0;

        /// Returns a heap-allocated copy of the concrete shape; caller takes ownership.
        virtual Shape *clone() const = 0;
    };
} // namespace engine
