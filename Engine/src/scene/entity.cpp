#include "scene/entity.hpp"

#include "scene/tagcomponent.hpp"

namespace engine
{
    void Entity::addTag(const std::string &tag)
    {
        this->add<TagComponent>(tag);
    }

    void Entity::removeTag()
    {
        this->remove<TagComponent>();
    }
} // namespace engine