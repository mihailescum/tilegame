#include "timer.hpp"

#include "components/timer.hpp"
#include "components/event.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    TimerSystem::TimerSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void TimerSystem::update(const engine::GameTime &update_time)
    {
        // Advance Timers
        const auto timer_view = _registry.view<components::Timer>(entt::exclude<components::Inactive>);
        for (auto &&[entity, timer] : timer_view.each())
        {
            timer.time_left -= update_time.elapsed_time;
            if (timer.time_left <= 0)
            {
                // Add an event
                _registry.emplace<components::TimerEventArgs>(entity, timer.time_total, timer.repeat);

                if (timer.repeat)
                {
                    timer.time_left += timer.time_total;
                }
                else
                {
                    _entities_to_clear.push_back(entity);
                }
            }

            // Trigger on_update()
            _registry.patch<components::Timer>(entity);
        }

        if (!_entities_to_clear.empty())
        {
            _registry.remove<components::Timer>(_entities_to_clear.begin(), _entities_to_clear.end());
            _entities_to_clear.clear();
        }

        // Trigger Timer Events
        const auto timer_event_view = _registry.view<components::TimerEventArgs>(entt::exclude<components::Inactive>);
        const auto timer_listener_view = _registry.view<components::EventListener<components::TimerEventArgs>>(entt::exclude<components::Inactive>);
        for (auto &&[source, event_args] : timer_event_view.each())
        {
            for (auto &&[listener, listener_component] : timer_listener_view.each())
            {
                if (!_registry.valid(listener_component.source) || listener_component.source == source)
                {
                    listener_component.callback(components::TimerEventArgs::EVENT_TYPE, event_args, source);
                }
            }
        }
    }
} // namespace tilegame::systems