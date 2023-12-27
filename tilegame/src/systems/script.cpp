#include "script.hpp"

#include "components/scriptloader.hpp"
#include "components/timer.hpp"
#include "components/luatable.hpp"
#include "components/inactive.hpp"
#include "components/target.hpp"
#include "components/velocity.hpp"

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
        _lua().new_usertype<glm::vec2>(
            "vec2",
            sol::constructors<glm::vec2(), glm::vec2(float, float), glm::vec2(const glm::vec2 &)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y);
        _lua().new_usertype<entt::entity>(
            "_entity", sol::constructors<entt::entity>());
        _lua().new_usertype<components::ScriptLoader>(
            "_ScriptLoaderComponent",
            sol::constructors<components::ScriptLoader(), components::ScriptLoader(const std::string)>(),
            "path", &components::ScriptLoader::path);
        _lua().new_usertype<components::Timer>(
            "_TimerComponent",
            sol::constructors<components::Timer(), components::Timer(float, bool)>(),
            "repeat", &components::Timer::repeat,
            "time_left", &components::Timer::time_left,
            "time_total", &components::Timer::time_total);
        _lua().new_usertype<components::TimerEventArgs>(
            "_TimerEventArgsComponent",
            sol::constructors<components::TimerEventArgs(), components::TimerEventArgs(float, bool)>(),
            "duration", &components::TimerEventArgs::duration,
            "repeated", &components::TimerEventArgs::repeated);
        _lua().new_usertype<components::LuaTable>(
            "_TableComponent",
            sol::constructors<components::LuaTable(), components::LuaTable(const sol::table &)>(),
            "table", &components::LuaTable::table);
        _lua().new_usertype<components::Inactive>(
            "_InactiveComponent",
            sol::constructors<components::Inactive()>());
        _lua().new_usertype<components::Target>(
            "_TargetComponent",
            sol::constructors<components::Target(), components::Target(glm::vec2)>(),
            "target", &components::Target::target);
        _lua().new_usertype<components::Velocity>(
            "_VelocityComponent",
            sol::constructors<components::Velocity(), components::Velocity(float)>(),
            "velocity", &components::Velocity::velocity);

        add_component_function<
            EmplaceOrReplaceWrapper,
            components::Timer,
            components::TimerEventArgs,
            components::ScriptLoader,
            components::LuaTable,
            components::Target,
            components::Velocity>("_add_component");

        _lua().set_function("_add_timer_event_listener", &ScriptSystem::add_event_listener<components::TimerEventArgs>, this);
    }

    entt::entity ScriptSystem::create_entity()
    {
        return _registry.create();
    }

    void ScriptSystem::update(const engine::GameTime &update_time)
    {
        // TODO loading scripts should happen somewhere else, not on update

        const auto script_view = _registry.view<components::ScriptLoader>(entt::exclude<components::Inactive>);
        for (const auto &&[entity, script_loader] : script_view.each())
        {
            const auto &script_path = script_loader.path;
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
