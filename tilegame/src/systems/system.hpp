#pragma once

#include <entt/entity/registry.hpp>

#include "engine.hpp"
#include "scenegraphdata.hpp"

#include "components/event.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    class System
    {
    protected:
        tilegame::Scene &_scene;
        entt::registry &_registry;

        template <class Event, class EventListener = components::EventListener<Event>>
        void raise_events()
        {
            const auto event_view = _registry.view<const Event>(entt::exclude<components::Inactive>);
            const auto listener_view = _registry.view<const EventListener>(entt::exclude<components::Inactive>);
            for (const auto &&[source, event] : event_view.each())
            {
                for (auto &&[listener, listener_component] : listener_view.each())
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