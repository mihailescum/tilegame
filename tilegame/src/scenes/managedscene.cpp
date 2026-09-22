#include "scenes/managedscene.hpp"

#include "tilegame.hpp"
#include "scenes/scenemanager.hpp"

namespace tilegame::scenes
{
    ManagedScene::ManagedScene(Tilegame &game, SceneManager &scene_manager)
        : tilegame::Scene(game), _scene_manager(scene_manager)
    {
    }

    void ManagedScene::close(std::any result)
    {
        _scene_manager.close(*this, std::move(result));
    }
} // namespace tilegame::scenes
