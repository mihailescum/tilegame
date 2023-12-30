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
    class Script : public System
    {
    private:
        tilegame::SecureLuaState _lua;
        std::unordered_map<std::string, std::function<bool(sol::function, entt::entity)>> _event_types;

        void register_api();
        bool add_event_listener(const sol::table &event, sol::function callback, entt::entity source);

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
