#include "script.hpp"

#include "debugger_lua/debugger_lua.h"

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
        open_libs();
        register_api();
        // dbg_setup(_lua, "debugger", "dbg", NULL, NULL);
    }

    void ScriptSystem::open_libs()
    {
        // See https://blog.rubenwardy.com/2020/07/26/sol3-script-sandbox/ for the setup of our sandbox

        // Create new blank environment
        auto env = sol::environment(_lua, sol::create);

        // Set global variable for globals
        env["_G"] = env;

        const std::vector<std::string> whitelisted = {
            "assert",
            "error",
            "ipairs",
            "next",
            "pairs",
            "pcall",
            "print",
            "select",
            "tonumber",
            "tostring",
            "type",
            "unpack",
            "_VERSION",
            "xpcall",

            // These functions are unsafe as they can bypass or change metatables,
            // but they are required to implement classes.
            "rawequal",
            "rawget",
            "rawset",
            "setmetatable",
        };

        for (const auto &name : whitelisted)
        {
            env[name] = _lua[name];
        }

        _lua.open_libraries(sol::lib::coroutine, sol::lib::string, sol::lib::table, sol::lib::math);
        const std::vector<std::string> safeLibraries = {
            "coroutine", "string", "table", "math"};

        for (const auto &name : safeLibraries)
        {
            sol::table copy(_lua, sol::create);
            sol::table library_table = _lua[name];
            for (auto pair : library_table)
            {
                // first is the name of a function in module, second is the function
                copy[pair.first] = pair.second;
            }
            env[name] = copy;
        }

        _lua.open_libraries(sol::lib::os);
        sol::table os(_lua, sol::create);
        sol::table os_lua = _lua["os"];
        os["clock"] = _lua["os"]["clock"];
        os["date"] = _lua["os"]["date"];
        os["difftime"] = _lua["os"]["difftime"];
        os["time"] = _lua["os"]["time"];

        // TODO maybe allow only in debug mode?
        os["exit"] = _lua["os"]["exit"];

        env["os"] = os;

        // Setting the global environment
#if LUA_VERSION_NUM >= 502
        // Get environment registry index
        lua_rawgeti(_lua, LUA_REGISTRYINDEX, env.registry_index());

        // Set the global environment
        lua_rawseti(_lua, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
#else
        // Get main thread
        int is_main = lua_pushthread(lua);
        assert(is_main);
        int thread = lua_gettop(lua);

        // Get environment registry index
        lua_rawgeti(lua, LUA_REGISTRYINDEX, env.registry_index());

        // Set the global environment
        if (!lua_setfenv(lua, thread))
        {
            throw ModException(
                "Security: Unable to set environment of the main Lua thread!");
        };
        lua_pop(lua, 1); // Pop thread
#endif
    }

    void ScriptSystem::register_api()
    {
        _lua.set_function("_create_entity", &ScriptSystem::create_entity, this);
        // Possible syntax to directly bind the _registry.create() function:
        // _lua.set_function("_create_entity_direct", (entt::entity(entt::registry::*)()) & entt::registry::create, &_registry);

        // Define the user types
        _lua.new_usertype<entt::entity>(
            "_entity", sol::constructors<entt::entity>());
        _lua.new_usertype<components::ScriptLoader>(
            "_ScriptLoaderComponent",
            sol::constructors<components::ScriptLoader(), components::ScriptLoader(const std::string)>(),
            "path", &components::ScriptLoader::path);
        _lua.new_usertype<components::Timer>(
            "_TimerComponent",
            sol::constructors<components::Timer(), components::Timer(double, bool)>(),
            "repeat", &components::Timer::repeat,
            "time_left", &components::Timer::time_left,
            "time_total", &components::Timer::time_total);
        _lua.new_usertype<components::TimerEventArgs>(
            "_TimerEventArgsComponent",
            sol::constructors<components::TimerEventArgs(), components::TimerEventArgs(double, bool)>(),
            "duration", &components::TimerEventArgs::duration,
            "repeated", &components::TimerEventArgs::repeated);
        _lua.new_usertype<components::LuaTable>(
            "_TableComponent",
            sol::constructors<components::LuaTable(), components::LuaTable(const sol::table &)>(),
            "table", &components::LuaTable::table);

        add_component_function<
            EmplaceOrReplaceWrapper,
            components::Timer,
            components::TimerEventArgs,
            components::ScriptLoader,
            components::LuaTable>("_add_component");

        _lua.set_function("_add_timer_event_listener", &ScriptSystem::add_event_listener<components::TimerEventArgs>, this);
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
            _lua.script_file(script_path);
            /*sol::load_result load_result = _lua.load_file(script_path);
            if (load_result.valid())
            {
                sol::table arg = _lua.create_table_with("entity", entity);
                sol::table result = load_result(arg);
                _registry.emplace<components::LuaTable>(entity, result);
            }
            else
            {
                throw "Error loading file";
            }*/

            _entities_to_clear.push_back(entity);
        }

        if (!_entities_to_clear.empty())
        {
            _registry.remove<components::ScriptLoader>(_entities_to_clear.begin(), _entities_to_clear.end());
            _entities_to_clear.clear();
        }
    }
} // namespace tilegame::systems
