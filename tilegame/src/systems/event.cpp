#include "event.hpp"

#include "components/timer.hpp"
#include "components/target.hpp"

namespace tilegame::systems
{
    Event::Event(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Event::update(const engine::GameTime &update_time)
    {
        // Clears all remaining events
        _registry.clear<components::TimerEvent, components::TargetReachedEvent>();
    }
} // namespace tilegame::systems
