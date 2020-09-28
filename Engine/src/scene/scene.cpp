#include "scene/scene.hpp"

#include "scene/entity.hpp"

namespace engine
{
    Scene::Scene()
    {
        this->registry.create();
    }

    const Entity Scene::createEntity()
    {
        entt::entity handle = this->registry.create();
        Entity result(handle, this);
        return result;
    }
} // namespace engine