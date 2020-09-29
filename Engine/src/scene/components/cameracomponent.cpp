#include "scene/components/positioncomponent.hpp"

namespace engine
{
    PositionComponent::PositionComponent() : PositionComponent(glm::vec2(0.0))
    {
    }

    PositionComponent::PositionComponent(const glm::vec2 &position) : position(position)
    {
    }
} // namespace engine