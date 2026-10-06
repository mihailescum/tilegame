#include "timer.hpp"

#include "components/timer.hpp"

namespace tilegame::systems
{
    Timer::Timer(engine::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Timer::update(const engine::GameTime &update_time)
    {
        // Advance Timers
        const auto timer_entities = _registry.view<components::Timer>(entt::exclude<engine::Inactive>);
        for (auto &&[entity, timer] : timer_entities.each())
        {
            timer.time_left -= update_time.elapsed_time;
            if (timer.time_left <= 0)
            {
                // Raised immediately: none of today's TimerEvent listeners add/remove a Timer
                // component (see System::raise_event()'s caution), only reschedule/mutate their
                // own via patch()/emplace_or_replace(), which is safe mid-iteration.
                raise_event<components::TimerEvent>(entity, entt::null, timer.time_total, timer.repeat);

                // Re-fetched rather than reusing `timer`: a listener may have replaced this
                // entity's Timer (e.g. systems::Lightning re-arming itself for the next strike)
                // or removed it outright.
                auto *current = _registry.try_get<components::Timer>(entity);
                if (!current)
                {
                    continue;
                }

                if (current->time_left > 0)
                {
                    // Re-armed by a listener: keep it, rather than erasing/rewinding it below.
                    _registry.patch<components::Timer>(entity);
                }
                else if (current->repeat)
                {
                    current->time_left += current->time_total;
                    // Trigger on_update()
                    _registry.patch<components::Timer>(entity);
                }
                else
                {
                    // No Timer left on `entity` to patch once erased.
                    _registry.erase<components::Timer>(entity);
                }
            }
            else
            {
                // Trigger on_update()
                _registry.patch<components::Timer>(entity);
            }
        }
    }
} // namespace tilegame::systems
