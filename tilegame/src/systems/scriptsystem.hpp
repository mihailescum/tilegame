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

        template <class T>
        T &emplace_or_replace_component(entt::entity entity, const T &component)
        {
            return _registry.emplace_or_replace<T>(entity, component);
        }

    public:
        ScriptSystem(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
