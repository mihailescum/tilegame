#include "scriptsystem.hpp"

namespace tilegame::systems
{
    ScriptSystem::ScriptSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void ScriptSystem::initialize()
    {
        _lua.open_libraries(sol::lib::base);
    }
} // namespace tilegame::systems
