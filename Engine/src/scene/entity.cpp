#include "scene/entity.hpp"

#include "core/log.hpp"
#include "scene/components/tagcomponent.hpp"

namespace engine
{
    Entity::Entity(entt::entity handle, Scene *scene) 
        : handle(handle), scene(scene)
    {
    }

    void Entity::addTag(const std::string &tag)
    {
        this->add<TagComponent>(tag);
    }

    void Entity::removeTag()
    {
        this->remove<TagComponent>();
    }
} // namespace engine