#include "event.hpp"

#include "components/timer.hpp"

namespace tilegame::systems
{
    EventSystem::EventSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void EventSystem::update(const engine::GameTime &update_time)
    {
        // Clears all remaining events
        _registry.clear<components::TimerEventArgs>();
    }
} // namespace tilegame::systems
