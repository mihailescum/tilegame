#pragma once

#include <glm/glm.hpp>

#include "glad/glad.h"

#include "core/shape.hpp"
#include "core/point.hpp"
#include "core/ray.hpp"
#include "core/log.hpp"

namespace engine
{
    /**
     * @brief Axis-aligned bounding box defined by a position and dimensions, used for layout, collision and ray-casting checks.
     */
    struct Rectangle : public Shape
    {
    public:
        static const Rectangle EMPTY;

        glm::vec2 position;
        glm::vec2 dimensions;

        Rectangle() : Rectangle(glm::vec2(0.0), glm::vec2(0.0)) {}
        Rectangle(const glm::vec2 &position, const glm::vec2 &dimensions) : position(position), dimensions(dimensions) {}

        inline Shape *clone() const override { return new Rectangle(*this); }

        bool intersects(const Rectangle &other) const;
        bool intersects(const Point &other) const;
        /**
         * @brief Performs a ray-AABB intersection test.
         * @param[out] contact_point Point where the ray hits the rectangle, if any.
         * @param[out] contact_normal Surface normal at the contact point, if any.
         * @param[out] t_hit_near Ray parameter (distance along direction) at the near hit, if any.
         * @return true if the ray intersects this rectangle.
         */
        bool intersects(const Ray &other, glm::vec2 &contact_point, glm::vec2 &contact_normal, float &t_hit_near) const;

        inline Rectangle operator+(const glm::vec2 &other) const
        {
            Rectangle result(position + other, dimensions);
            return result;
        }

        inline Rectangle operator-(const glm::vec2 &other) const
        {
            Rectangle result = *this + (-other);
            return result;
        }
    };
} // namespace engine
