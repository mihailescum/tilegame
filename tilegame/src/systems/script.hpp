#pragma once

#include <vector>
#include <unordered_map>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "engine.hpp"

#include "secureluastate.hpp"
#include "system.hpp"
#include "components/event.hpp"

namespace tilegame::systems
{
    /**
     * @brief Bridges the ECS/game to sandboxed per-entity Lua scripts.
     *
     * Owns the SecureLuaState, registers ECS components as Lua usertypes and
     * the native event types (TargetReachedEvent, TimerEvent) scripts can
     * subscribe to. Each frame, operates on entities with a ScriptLoader
     * component: loads and runs their Lua file once (passing the owning
     * entity), attaches the returned table as a LuaTable component, and
     * removes the ScriptLoader so the script isn't re-run.
     */
    class Script : public System
    {
    private:
        tilegame::SecureLuaState _lua;
        // Maps a Lua-visible EVENT_TYPE name to a function that creates the matching native
        // EventListener<EventType> entity, so add_event_listener can dispatch generically
        // without knowing the concrete event type at the call site.
        std::unordered_map<std::string, std::function<bool(sol::function, entt::entity)>> _event_types;

        void register_api();
        // Exposed to Lua as `_add_event_listener`; looks up the event's EVENT_TYPE in
        // _event_types and, if registered, creates the corresponding listener entity.
        bool add_event_listener(const sol::table &event, sol::function callback);
        // Exposed to Lua as `_add_event_listener`; looks up the event's EVENT_TYPE in
        // _event_types and, if registered, creates the corresponding listener entity.
        bool add_event_listener(const sol::table &event, sol::function callback, entt::entity source);
        // Exposed to Lua as `_to_global`; converts coordinates relative to the named
        // map's origin into world-space coordinates via the loaded World resource.
        glm::vec2 to_global(const std::string &map_name, const glm::vec2 &relative_position) const;
        // Exposed to Lua as `_show_message`; creates a components::MessageBox request entity that
        // systems::MessageBox picks up next frame, returning it so scripts can filter
        // `_MessageClosedEvent` by source if they want.
        entt::entity show_message(const std::string &text);

        // Registers EventType in _event_types so Lua scripts can listen for it via
        // add_event_listener; instantiated once per native event type in register_api().
        template <class EventType, class EventListener = components::EventListener<EventType>>
        void register_event_type()
        {
            _event_types[EventType::EVENT_TYPE] =
                [&_registry = _registry](sol::function callback, entt::entity source)
            {
                const auto listener_entity = _registry.create();
                _registry.emplace<EventListener>(listener_entity, EventListener(callback, source));
                return true;
            };
        }

    public:
        Script(tilegame::Scene &scene, entt::registry &registry);
        ~Script();

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
