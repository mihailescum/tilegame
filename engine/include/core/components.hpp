#pragma once

#include <sstream>

#include "entt/entt.hpp"

namespace engine
{
    /**
     * @brief Wraps a callback (typically a Lua function bound via Script::add_event_listener) that is
     * invoked by System::raise_event for event type `T` (e.g. TimerEvent, TargetReachedEvent).
     * Events are a communication between (up to) two entities - a `source` and a `target` - and
     * this listener can be scoped to either, both, or neither: if `source_filter` is set, the
     * callback only fires for events raised with that specific entity as their source; if
     * `target_filter` is set, only for events raised with that specific entity as their target.
     * Either left as entt::null (the default) matches any entity on that side.
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
        std::function<void(const std::string, const T &, entt::entity, entt::entity)> callback;
        /// Entity to filter events by source; entt::null means listen to events from any source.
        entt::entity source_filter;
        /// Entity to filter events by target; entt::null means listen to events with any target.
        entt::entity target_filter;

        EventListener() : source_filter(entt::null), target_filter(entt::null) {}
        EventListener(std::function<void(const std::string, const T &, entt::entity, entt::entity)> callback) : callback(callback), source_filter(entt::null), target_filter(entt::null) {}
        EventListener(std::function<void(const std::string, const T &, entt::entity, entt::entity)> callback, entt::entity source) : callback(callback), source_filter(source), target_filter(entt::null) {}
        EventListener(std::function<void(const std::string, const T &, entt::entity, entt::entity)> callback, entt::entity source, entt::entity target) : callback(callback), source_filter(source), target_filter(target) {}

        void operator()(const std::string type, const T &event, entt::entity source, entt::entity target) const
        {
            if ((this->source_filter == entt::null || source == this->source_filter) &&
                (this->target_filter == entt::null || target == this->target_filter))
            {
                callback(type, event, source, target);
            }
        }
    };

    struct Inactive
    {
        Inactive() {}

        [[nodiscard]] std::string to_string() const
        {
            std::stringstream ss;
            ss << "Inactive";
            return ss.str();
        }
    };
} // namespace engine
