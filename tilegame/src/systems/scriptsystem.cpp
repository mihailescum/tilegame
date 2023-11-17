#include "scriptsystem.hpp"

#include "components/scriptloader.hpp"

namespace tilegame::systems
{
    ScriptSystem::ScriptSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void ScriptSystem::initialize()
    {
        _lua.open_libraries(sol::lib::base);
        register_api();
    }

    void ScriptSystem::register_api()
    {
        _lua.set_function("_create_entity", &ScriptSystem::create_entity, this);
        // _lua.set_function("_create_entity_direct", (entt::entity(entt::registry::*)()) & entt::registry::create, &_registry);
    }

    entt::entity ScriptSystem::create_entity()
    {
        return _registry.create();
    }

    void ScriptSystem::update(const engine::GameTime &update_time)
    {
        const auto script_view = _registry.view<tilegame::components::ScriptLoader>();
        for (const auto &&[entity, script] : script_view.each())
        {
            const auto &script_path = script.path;
            _lua.script_file(script_path);
        }
        _registry.clear<tilegame::components::ScriptLoader>();
    }
} // namespace tilegame::systems
