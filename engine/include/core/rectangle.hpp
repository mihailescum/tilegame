#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "core/shape.hpp"
#include "core/point.hpp"
#include "core/ray.hpp"
#include "core/log.hpp"

namespace engine
{
    struct Rectangle : public Shape
    {
    public:
        static const Rectangle EMPTY;
        GLfloat x;
        GLfloat y;
        GLfloat width;
        GLfloat height;

        Rectangle() : Rectangle(0.0f, 0.0f, 0.0f, 0.0f) {}
        Rectangle(GLfloat x, GLfloat y, GLfloat width, GLfloat height) : x(x), y(y), width(width), height(height) {}

        inline Shape *clone() const override { return new Rectangle(*this); }

        bool intersects(const Rectangle &other) const;
        bool intersects(const Point &other) const;
        bool intersects(const Ray &other, glm::vec2 &contact_point, glm::vec2 &contact_normal, float &t_hit_near) const;

        inline Rectangle operator+(const glm::vec2 &other) const
        {
            Rectangle result(x + other.x, y + other.y, width, height);
            return result;
        }

        inline Rectangle operator-(const glm::vec2 &other) const
        {
            Rectangle result = *this + (-other);
            return result;
        }
    };
} // namespace engine
