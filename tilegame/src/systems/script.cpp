#include "script.hpp"

#include "components/animation.hpp"
#include "components/camera.hpp"
#include "components/movement.hpp"
#include "components/ordering.hpp"
#include "components/player.hpp"
#include "components/renderable2d.hpp"
#include "components/scenenode.hpp"
#include "components/scriptloader.hpp"
#include "components/sprite.hpp"
#include "components/transform.hpp"
#include "components/timer.hpp"

namespace tilegame::systems
{
    ScriptSystem::ScriptSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void ScriptSystem::initialize()
    {
        _lua.open_libraries(sol::lib::base, sol::lib::coroutine);
        register_api();
    }

    void ScriptSystem::register_api()
    {
        _lua.set_function("_create_entity", &ScriptSystem::create_entity, this);
        // Possible syntax to directly bind the _registry.create() function:
        // _lua.set_function("_create_entity_direct", (entt::entity(entt::registry::*)()) & entt::registry::create, &_registry);

        _lua.new_usertype<entt::entity>("_entity", sol::constructors<entt::entity>());
        _lua.new_usertype<tilegame::components::ScriptLoader>("_ScriptLoaderComponent", sol::constructors<tilegame::components::ScriptLoader(), tilegame::components::ScriptLoader(const std::string)>());
        _lua.new_usertype<tilegame::components::Timer>("_TimerComponent", sol::constructors<tilegame::components::Timer(), tilegame::components::Timer(double)>());

        add_component_function<
            EmplaceOrReplaceWrapper,
            tilegame::components::Timer,
            tilegame::components::ScriptLoader>("_add_component");
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
            sol::table result = _lua.script_file(script_path);
            result["main"]();

            _entities_to_clear.push_back(entity);
        }

        if (!_entities_to_clear.empty())
        {
            _registry.remove<tilegame::components::ScriptLoader>(_entities_to_clear.begin(), _entities_to_clear.end());
            _entities_to_clear.clear();
        }
    }
} // namespace tilegame::systems
