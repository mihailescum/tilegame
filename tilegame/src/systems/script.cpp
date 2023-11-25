#include "script.hpp"

#include "components/scriptloader.hpp"
#include "components/timer.hpp"
#include "components/luatable.hpp"

namespace tilegame::systems
{
    ScriptSystem::ScriptSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    ScriptSystem::~ScriptSystem()
    {
        // Clear all components before the lua state is destroyed to avoid invalid references which would lead to a SegFault
        _registry.clear();
    }

    void ScriptSystem::initialize()
    {
        _lua.open_libraries();
        register_api();
    }

    void ScriptSystem::register_api()
    {
        _lua().set_function("_create_entity", &ScriptSystem::create_entity, this);
        // Possible syntax to directly bind the _registry.create() function:
        // _lua().set_function("_create_entity_direct", (entt::entity(entt::registry::*)()) & entt::registry::create, &_registry);

        // Define the user types
        _lua().new_usertype<entt::entity>(
            "_entity", sol::constructors<entt::entity>());
        _lua().new_usertype<components::ScriptLoader>(
            "_ScriptLoaderComponent",
            sol::constructors<components::ScriptLoader(), components::ScriptLoader(const std::string)>(),
            "path", &components::ScriptLoader::path);
        _lua().new_usertype<components::Timer>(
            "_TimerComponent",
            sol::constructors<components::Timer(), components::Timer(double, bool)>(),
            "repeat", &components::Timer::repeat,
            "time_left", &components::Timer::time_left,
            "time_total", &components::Timer::time_total);
        _lua().new_usertype<components::TimerEventArgs>(
            "_TimerEventArgsComponent",
            sol::constructors<components::TimerEventArgs(), components::TimerEventArgs(double, bool)>(),
            "duration", &components::TimerEventArgs::duration,
            "repeated", &components::TimerEventArgs::repeated);
        _lua().new_usertype<components::LuaTable>(
            "_TableComponent",
            sol::constructors<components::LuaTable(), components::LuaTable(const sol::table &)>(),
            "table", &components::LuaTable::table);

        add_component_function<
            EmplaceOrReplaceWrapper,
            components::Timer,
            components::TimerEventArgs,
            components::ScriptLoader,
            components::LuaTable>("_add_component");

        _lua().set_function("_add_timer_event_listener", &ScriptSystem::add_event_listener<components::TimerEventArgs>, this);
    }

    entt::entity ScriptSystem::create_entity()
    {
        return _registry.create();
    }

    void ScriptSystem::update(const engine::GameTime &update_time)
    {
        const auto script_view = _registry.view<components::ScriptLoader>();
        for (const auto &&[entity, script] : script_view.each())
        {
            const auto &script_path = script.path;
            sol::load_result load_result = _lua().load_file(script_path);
            if (load_result.valid())
            {
                sol::table arg = _lua().create_table_with("entity", entity);
                sol::table result = load_result(arg);
                _registry.emplace<components::LuaTable>(entity, result);
            }
            else
            {
                throw "Error loading file";
            }

            _entities_to_clear.push_back(entity);
        }

        if (!_entities_to_clear.empty())
        {
            _registry.remove<components::ScriptLoader>(_entities_to_clear.begin(), _entities_to_clear.end());
            _entities_to_clear.clear();
        }
    }
} // namespace tilegame::systems
