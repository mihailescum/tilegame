#include "event.hpp"

namespace tilegame::systems
{
    EventSystem::EventSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void EventSystem::update(const engine::GameTime &update_time)
    {
    }
} // namespace tilegame::systems
