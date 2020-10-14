#include "scene/scene.hpp"

#include "scene/entity.hpp"

namespace engine
{
    Scene::Scene(Game &game) : game(game)
    {
        this->registry.create();
    }

    Entity Scene::createEntity()
    {
        entt::entity handle = this->registry.create();
        Entity result(handle, this);
        return result;
    }

    entt::registry &Scene::getRegistry() { return this->registry; }
    const Game &Scene::getGame() const { return this->game; }
} // namespace engine