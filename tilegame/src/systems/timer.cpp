#include "timer.hpp"

#include "components/timer.hpp"

namespace tilegame::systems
{
    TimerSystem::TimerSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void TimerSystem::update(const engine::GameTime &update_time)
    {
        const auto timer_view = _registry.view<tilegame::components::Timer>();

        for (auto &&[entity, timer] : timer_view.each())
        {
            timer.time_left -= update_time.elapsed_time;
            if (timer.time_left <= 0)
            {
                // TODO trigger event
                _entities_to_clear.push_back(entity);
            }

            // Trigger on_update()
            _registry.patch<tilegame::components::Timer>(entity);
        }

        if (!_entities_to_clear.empty())
        {
            _registry.remove<tilegame::components::Timer>(_entities_to_clear.begin(), _entities_to_clear.end());
            _entities_to_clear.clear();
        }
    }
} // namespace tilegame::systems
