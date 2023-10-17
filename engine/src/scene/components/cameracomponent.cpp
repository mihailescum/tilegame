#include "scene/components/positioncomponent.hpp"

namespace engine
{
    PositionComponent::PositionComponent() : PositionComponent(glm::dvec2(0.0))
    {
    }

    PositionComponent::PositionComponent(const glm::dvec2 &position) : position(position)
    {
    }
} // namespace engine