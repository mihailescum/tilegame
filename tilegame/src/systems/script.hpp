#pragma once

#include <vector>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "engine.hpp"

#include "secureluastate.hpp"
#include "system.hpp"
#include "components/event.hpp"

namespace tilegame::systems
{
    class ScriptSystem : public System
    {
    private:
        tilegame::SecureLuaState _lua;
        std::vector<entt::entity> _entities_to_clear;

        void register_api();
        void open_libs();
        entt::entity create_entity();

        struct EmplaceOrReplaceWrapper
        {
            EmplaceOrReplaceWrapper(ScriptSystem &parent) : _parent(parent) {}

            template <typename T>
            T &operator()(entt::entity entity, const T &component) const
            {
                return _parent._registry.emplace_or_replace<T>(entity, component);
            }

        private:
            ScriptSystem &_parent;
        };

        template <typename F, typename... Ts>
        void add_component_function(const std::string name)
        {
            auto overload = sol::overload(
                [this](entt::entity entity, Ts component)
                { return F(*this)(entity, component); }...);
            _lua().set_function(name, overload);
        }

        template <class T>
        void add_event_listener(std::function<void(const std::string, const T &, entt::entity)> callback, entt::entity source)
        {
            const auto listener_entity = _registry.create();
            _registry.emplace<components::EventListener<T>>(listener_entity, components::EventListener(callback, source));
        }

    public:
        ScriptSystem(tilegame::Scene &scene, entt::registry &registry);
        ~ScriptSystem();

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
