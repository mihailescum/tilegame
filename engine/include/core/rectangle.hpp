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

        glm::vec2 position;
        glm::vec2 size;

        Rectangle() : Rectangle(glm::vec2(0.0), glm::vec2(0.0)) {}
        Rectangle(const glm::vec2 &position, const glm::vec2 &size) : position(position), size(size) {}

        inline Shape *clone() const override { return new Rectangle(*this); }

        bool intersects(const Rectangle &other) const;
        bool intersects(const Point &other) const;
        bool intersects(const Ray &other, glm::vec2 &contact_point, glm::vec2 &contact_normal, float &t_hit_near) const;

        inline Rectangle operator+(const glm::vec2 &other) const
        {
            Rectangle result(position + other, size);
            return result;
        }

        inline Rectangle operator-(const glm::vec2 &other) const
        {
            Rectangle result = *this + (-other);
            return result;
        }
    };
} // namespace engine
