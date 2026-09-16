#pragma once

#include <string>

#include "entt/entt.hpp"

namespace tilegame::components
{
    /**
     * @brief Wraps a callback (typically a Lua function bound via Script::add_event_listener) that is
     * invoked by System::raise_event for event type `T` (e.g. TimerEvent, TargetReachedEvent).
     * If `source` is set, the callback only fires for events raised with that specific entity as
     * their source; otherwise it fires for events from any source.
     *
     * Caution: `System::raise_event()`'s listener lookup excludes entities tagged Inactive.
     * Don't put an `EventListener<T>` meant to *remove* an entity's Inactive tag (e.g. "start"
     * events like SetLightningEvent/ShakeCameraHorizontalEvent) on that same entity - it would
     * never fire, since the entity is exactly the thing being excluded. Put it on a separate,
     * always-active entity instead (see systems::Lightning/Weather/Camera's `control_entity`/
     * `weather_entity` for the pattern).
     */
    template <class T>
    struct EventListener
    {
        std::function<void(const std::string, const T &, entt::entity)> callback;
        /// Entity to filter events by; entt::null means listen to events from any entity.
        entt::entity target_entity;

        EventListener() : target_entity(entt::null) {}
        EventListener(std::function<void(const std::string, const T &, entt::entity)> callback) : callback(callback), target_entity(entt::null) {}
        EventListener(std::function<void(const std::string, const T &, entt::entity)> callback, entt::entity source) : callback(callback), target_entity(source) {}

        void operator()(const std::string type, const T &event, entt::entity source) const
        {
            if (this->target_entity == entt::null || source == this->target_entity)
            {
                callback(type, event, source);
            }
        }
    };
} // namespace tilegame::components
