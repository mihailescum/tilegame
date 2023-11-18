#pragma once

#define SOL_ALL_SAFETIES_ON 1
#pragma once

#include <vector>

#include "sol/sol.hpp"

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class ScriptSystem : public System
    {
    private:
        sol::state _lua;
        std::vector<entt::entity> _entities_to_clear;

        void register_api();
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
            _lua.set_function(name, overload);
        }

    public:
        ScriptSystem(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
