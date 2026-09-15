#pragma once

#include "entt/entt.hpp"

#include "engine.hpp"
#include "scenegraphdata.hpp"

#include "components/event.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    /**
     * @brief Common base class for all per-frame ECS game-behavior systems.
     *
     * Gives derived systems access to the owning Scene and the shared
     * entt::registry, plus the raise_events() dispatch helper. WorldScene
     * owns one instance of each System subclass and calls their lifecycle
     * methods (initialize/load_content/update/end_update/draw, as
     * implemented) in a fixed order each frame; this base class does not
     * enforce that order itself.
     */
    class System
    {
    protected:
        tilegame::Scene &_scene;
        entt::registry &_registry;

        // Delivers every entity carrying an Event-tag component (e.g. TargetReachedEvent,
        // TimerEvent) to every entity carrying a matching EventListener<Event> component,
        // by invoking the listener with the event's EVENT_TYPE, data and source entity. Used
        // to notify Lua scripts (via systems::Script) of native engine events.
        template <class Event, class EventListener = components::EventListener<Event>>
        void raise_events() const
        {
            const auto event_entities = _registry.view<const Event>(entt::exclude<components::Inactive>);
            const auto listener_entities = _registry.view<const EventListener>(entt::exclude<components::Inactive>);
            for (const auto &&[source, event] : event_entities.each())
            {
                for (auto &&[listener, listener_component] : listener_entities.each())
                {
                    listener_component(Event::EVENT_TYPE, event, source);
                }
            }
        }

    public:
        System(tilegame::Scene &scene, entt::registry &registry);
        virtual ~System() = 0;
    };
} // namespace tilegame