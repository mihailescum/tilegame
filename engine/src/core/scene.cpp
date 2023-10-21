#include "core/scene.hpp"

namespace engine
{
    Scene::Scene(Game &game) : _game(game)
    {
    }

    const Game &Scene::get_game() const { return this->_game; }
} // namespace engine