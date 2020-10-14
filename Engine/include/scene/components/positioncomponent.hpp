#pragma once

#include <glm/glm.hpp>

namespace engine
{
    struct PositionComponent
    {
        glm::vec2 position;

        PositionComponent();
        PositionComponent(const PositionComponent &) = default;
        PositionComponent(const glm::vec2 &position);

        const float x() const { return position.x; }
        const float y() const { return position.y; }

        operator glm::vec2 &() { return this->position; }
        operator const glm::vec2 &() const { return this->position; }
    };
} // namespace engine