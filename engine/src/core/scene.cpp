#include "core/scene.hpp"

namespace engine
{
    Scene::Scene(Game &game) : game(game)
    {
    }

    const Game &Scene::getGame() const { return this->game; }
} // namespace engine