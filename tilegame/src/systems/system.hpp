#pragma once

#include <type_traits>

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
        //
        // Event types with no data fields (e.g. MessageClosedEvent) are treated by entt as
        // empty/tag types, so their view's each() yields just the entity rather than
        // (entity, event) - handled below via is_empty_v instead of relying on each()'s shape.
        template <class Event, class EventListener = components::EventListener<Event>>
        void raise_events() const
        {
            const auto event_entities = _registry.view<const Event>(entt::exclude<components::Inactive>);
            const auto listener_entities = _registry.view<const EventListener>(entt::exclude<components::Inactive>);
            for (const auto source : event_entities)
            {
                for (const auto listener : listener_entities)
                {
                    const auto &listener_component = listener_entities.template get<const EventListener>(listener);
                    if constexpr (std::is_empty_v<Event>)
                    {
                        listener_component(Event::EVENT_TYPE, Event{}, source);
                    }
                    else
                    {
                        const auto &event = event_entities.template get<const Event>(source);
                        listener_component(Event::EVENT_TYPE, event, source);
                    }
                }
            }
        }

    public:
        System(tilegame::Scene &scene, entt::registry &registry);
        virtual ~System() = 0;
    };
} // namespace tilegame