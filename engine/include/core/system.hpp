#pragma once

#include <vector>

#include "entt/entt.hpp"

#include "scene.hpp"
#include "components.hpp"

namespace engine
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
        Scene &_scene;
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
        // Requires `Event::EVENT_TYPE` to exist (see engine::EventListener<T>), even for
        // events with no Lua usertype of their own - delivery needs it regardless of who's
        // listening.
        //
        // Listeners may add or remove listeners - including themselves, e.g. a one-shot Lua
        // callback calling `_remove_event_listener` on its own handle - while this is
        // delivering: the listener set is snapshotted up front (so listeners added meanwhile
        // only get the *next* event), any removed or deactivated before their turn are skipped,
        // and each listener is copied before being called, so destroying it mid-call doesn't
        // destroy the callback that's still executing.
        //
        // Caution: since this calls arbitrary listener callbacks synchronously, calling it from
        // inside a view/each() loop over component types a listener might structurally add or
        // remove (as opposed to just modifying values of) can invalidate that iteration. Safe
        // for today's listeners; a new one that does this would need the raising loop to finish
        // first.
        template <class Event, class EventListener = EventListener<Event>, class... Args>
        void raise_event(entt::entity source = entt::null, entt::entity target = entt::null, Args &&...args) const
        {
            const Event event{std::forward<Args>(args)...};
            const auto listener_view = _registry.view<const EventListener>(entt::exclude<Inactive>);
            const std::vector<entt::entity> listener_entities(listener_view.begin(), listener_view.end());
            for (const auto listener : listener_entities)
            {
                if (!_registry.valid(listener) || !_registry.all_of<EventListener>(listener) || _registry.all_of<Inactive>(listener))
                {
                    continue;
                }

                const EventListener callback = _registry.get<const EventListener>(listener);
                callback(Event::EVENT_TYPE, event, source, target);
            }
        }

    public:
        System(Scene &scene, entt::registry &registry);
        virtual ~System() = 0;
    };
} // namespace tilegame