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
    class Script : public System
    {
    private:
        tilegame::SecureLuaState _lua;

        void register_api();
        entt::entity create_entity();
        bool add_event_listener(std::string event_type, sol::function callback, entt::entity source);

        struct EmplaceOrReplaceWrapper
        {
            EmplaceOrReplaceWrapper(Script &parent) : _parent(parent) {}

            template <typename T>
            T &operator()(entt::entity entity, const T &component) const
            {
                return _parent._registry.emplace_or_replace<T>(entity, component);
            }

        private:
            Script &_parent;
        };

        template <typename F, typename... Ts>
        void add_component_function(const std::string name)
        {
            auto overload = sol::overload(
                [this](entt::entity entity, Ts component)
                { return F(*this)(entity, component); }...);
            _lua().set_function(name, overload);
        }

    public:
        Script(tilegame::Scene &scene, entt::registry &registry);
        ~Script();

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
