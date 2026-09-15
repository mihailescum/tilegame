#pragma once

#include <string>

#include "entt/entt.hpp"

namespace tilegame::components
{
    /**
     * @brief Wraps a callback (typically a Lua function bound via Script::add_event_listener) that is
     * invoked by System::raise_events for every entity carrying an event component of type `T` (e.g.
     * TimerEvent, TargetReachedEvent). If `source` is set, the callback only fires for events raised
     * on that specific entity; otherwise it fires for events from any entity.
     */
    template <class T>
    struct EventListener
    {
        std::function<void(const std::string, const T &, entt::entity)> callback;
        /// Entity to filter events by; entt::null means listen to events from any entity.
        entt::entity source;

        EventListener() : source(entt::null) {}
        EventListener(std::function<void(const std::string, const T &, entt::entity)> callback) : callback(callback), source(entt::null) {}
        EventListener(std::function<void(const std::string, const T &, entt::entity)> callback, entt::entity source) : callback(callback), source(source) {}

        void operator()(const std::string type, const T &event, entt::entity source) const
        {
            if (source == entt::null || source == this->source)
            {
                callback(type, event, source);
            }
        }
    };
} // namespace tilegame::components
