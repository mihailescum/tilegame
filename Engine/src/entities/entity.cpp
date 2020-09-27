#include "entities/entity.hpp"

namespace engine
{
    Entity::Entity() : position(0.0), Scriptable("")
    {
    }

    const glm::vec2 &Entity::getPosition() const { return this->position; }
} // namespace engine