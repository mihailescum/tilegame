#include "movementsystem.hpp"

namespace tilegame::systems
{
    MovementSystem::MovementSystem(engine::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void MovementSystem::update(const engine::GameTime &update_time)
    {
    }
} // namespace tilegame::systems
