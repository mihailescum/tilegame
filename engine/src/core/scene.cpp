#include "core/scene.hpp"

#include "core/game.hpp"
#include "core/scenemanager.hpp"

namespace engine
{
    SceneManager &Scene::scene_manager()
    {
        return _game.scene_manager();
    }

    void Scene::close(std::any result)
    {
        scene_manager().close(*this, std::move(result));
    }
} // namespace engine
