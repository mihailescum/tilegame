#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    class ScriptSystem : public System
    {
    private:
        sol::state _lua;

    public:
        ScriptSystem(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
    };
} // namespace tilegame::systems
