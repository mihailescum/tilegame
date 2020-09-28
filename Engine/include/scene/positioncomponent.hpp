#pragma once

#include <glm/glm.hpp>

namespace engine
{
    struct PositionComponent
    {
        glm::vec2 position{0.0};

        PositionComponent() = default;
        PositionComponent(const PositionComponent &) = default;
        PositionComponent(const glm::vec2 &position)
            : position(position) {}

        operator glm::vec2 &() { return this->position; }
        operator const glm::vec2 &() const { return this->position; }
    };
} // namespace engine