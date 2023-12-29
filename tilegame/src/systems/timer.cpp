#include "timer.hpp"

#include "components/timer.hpp"
#include "components/event.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    Timer::Timer(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Timer::update(const engine::GameTime &update_time)
    {
        // Advance Timers
        const auto timer_view = _registry.view<components::Timer>(entt::exclude<components::Inactive>);
        for (auto &&[entity, timer] : timer_view.each())
        {
            timer.time_left -= update_time.elapsed_time;
            if (timer.time_left <= 0)
            {
                // Add an event
                _registry.emplace<components::TimerEvent>(entity, timer.time_total, timer.repeat);

                if (timer.repeat)
                {
                    timer.time_left += timer.time_total;
                }
                else
                {
                    _registry.erase<components::Timer>(entity);
                }
            }

            // Trigger on_update()
            _registry.patch<components::Timer>(entity);
        }

        raise_events<components::TimerEvent>();
    }

    void Timer::end_update()
    {
        _registry.clear<components::TimerEvent>();
    }
} // namespace tilegame::systems
