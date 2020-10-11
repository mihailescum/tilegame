#include "scene/entity.hpp"

#include "core/log.hpp"
#include "scene/components/tagcomponent.hpp"

namespace engine
{
    Entity::Entity(entt::entity handle, Scene *scene) 
        : handle(handle), scene(scene)
    {
    }
} // namespace engine