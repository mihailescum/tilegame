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
     * entt::registry, plus the raise_event()/raise() dispatch helpers. WorldScene
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

        // Builds an Event from `args` and immediately delivers it to every entity carrying a
        // matching EventListener<Event>, passing (EVENT_TYPE, event, source, target). `event` is
        // a plain local value here - never an entt component - so raising is always synchronous
        // and there is nothing for anyone to clean up afterwards: subscribers only ever see it
        // via their EventListener<Event> callback's `event` parameter, for the duration of this
        // call. `source` and `target`, if given, are whichever entities the event is conceptually
        // between (e.g. `source` the entity whose Timer just rang, or the NPC an InteractEvent
        // was raised on; `target` the player entity that triggered it); pass entt::null for
        // either side that isn't about any particular entity.
        //
        // Requires `Event::EVENT_TYPE` to exist (see components::EventListener<T>), even for
        // events with no Lua usertype of their own - delivery needs it regardless of who's
        // listening.
        //
        // Caution: since this calls arbitrary listener callbacks synchronously, calling it from
        // inside a view/each() loop over component types a listener might structurally add or
        // remove (as opposed to just modifying values of) can invalidate that iteration. Safe
        // for today's listeners; a new one that does this would need the raising loop to finish
        // first.
        template <class Event, class EventListener = components::EventListener<Event>, class... Args>
        void raise_event(entt::entity source = entt::null, entt::entity target = entt::null, Args &&...args) const
        {
            const Event event{std::forward<Args>(args)...};
            const auto listener_entities = _registry.view<const EventListener>(entt::exclude<components::Inactive>);
            for (const auto listener : listener_entities)
            {
                listener_entities.template get<const EventListener>(listener)(Event::EVENT_TYPE, event, source, target);
            }
        }

    public:
        System(tilegame::Scene &scene, entt::registry &registry);
        virtual ~System() = 0;
    };
} // namespace tilegame