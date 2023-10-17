#pragma once

#include <glm/glm.hpp>

namespace engine
{
    struct PositionComponent
    {
        glm::dvec2 position;

        PositionComponent();
        PositionComponent(const PositionComponent &) = default;
        PositionComponent(const glm::dvec2 &position);

        const double x() const { return position.x; }
        const double y() const { return position.y; }

        operator glm::dvec2 &() { return this->position; }
        operator const glm::dvec2 &() const { return this->position; }
    };
} // namespace engine