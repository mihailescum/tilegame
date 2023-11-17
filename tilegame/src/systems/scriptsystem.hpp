#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class ScriptSystem : public System
    {
    private:
        sol::state _lua;
        void register_api();
        entt::entity create_entity();

    public:
        ScriptSystem(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
