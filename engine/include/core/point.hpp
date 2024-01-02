#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "shape.hpp"

namespace engine
{
    struct Point : public Shape
    {
    public:
        Point() : Point(glm::vec2(0.0)) {}
        Point(const glm::vec2 &position) : position(position) {}

        glm::vec2 position;

        inline Shape *clone() const override { return new Point(*this); }

        static const Point ZERO;

        inline Point operator+(const glm::vec2 &other) const
        {
            Point result(position + other);
            return result;
        }

        inline Point operator-(const glm::vec2 &other) const
        {
            Point result = *this + (-other);
            return result;
        }
    };
} // namespace engine
