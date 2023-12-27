#include "secureluastate.hpp"

#include <iostream>

#include "debugger_lua/debugger_lua.hpp"
#include "inspect.lua/inspect.lua.hpp"

namespace tilegame
{
    sol::object SecureLuaState::safe_require(const std::string &name)
    {
        sol::table result;
        if (name == "debugger")
        {
            // TODO: something is not quite right with the debugger. 'e' or 'p' cannot access the local variables for some reason
            result = _lua.require_script(name, debugger_lua::debugger_src);
            result.set_function("write", &SecureLuaState::safe_write, this);
            result.set_function("read", &SecureLuaState::safe_read, this);
        }
        else if (name == "inspect")
        {
            // TODO: something is not quite right with the debugger. 'e' or 'p' cannot access the local variables for some reason
            result = _lua.require_script(name, inspect_lua::src);
        }
        else
        {
            throw "Unsupported module: " + name + "!";
        }

        return result;
    }

    void SecureLuaState::safe_write(const std::string &prompt)
    {
        std::cout << prompt << std::endl;
    }

    std::string SecureLuaState::safe_read(const std::string &prompt)
    {
        std::cout << prompt << ": ";

        std::string result;
        std::getline(std::cin, result);

        return result;
    }

    std::tuple<sol::object, sol::object> SecureLuaState::safe_load(const std::string &str, const std::string &chunkname)
    {
        if (!str.empty() && str[0] == LUA_SIGNATURE[0])
        {
            return std::make_tuple(sol::nil,
                                   sol::make_object(_lua, "Bytecode prohibited by Lua sandbox"));
        }

        sol::load_result result = _lua.load(str, chunkname, sol::load_mode::text);
        if (result.valid())
        {
            sol::function func = result;
            _env.set_on(func);
            return std::make_tuple(func, sol::nil);
        }
        else
        {
            return std::make_tuple(
                sol::nil, sol::make_object(_lua, ((sol::error)result).what()));
        }
    }

    void SecureLuaState::open_libraries()
    {
        // Create new blank _environment
        _env = sol::environment(_lua, sol::create);

        // Set global variable for globals
        _env["_G"] = _env;

        _lua.open_libraries(sol::lib::base);
        for (const auto &name : _whitelisted_functions)
        {
            _env[name] = _lua[name];
        }

        for (const auto &[name, lib] : _whitelisted_modules)
        {
            _lua.open_libraries(lib);

            sol::table copy(_lua, sol::create);
            sol::table library_table = _lua[name];
            for (auto pair : library_table)
            {
                // first is the name of a function in module, second is the function
                copy[pair.first] = pair.second;
            }
            _env[name] = copy;
        }

        _lua.open_libraries(sol::lib::os);
        sol::table os(_lua, sol::create);
        for (const auto &name : _whitelisted_os_functions)
        {
            os[name] = _lua["os"][name];
        }
        _env["os"] = os;

        // Set safe require function
        _env.set_function("require", &SecureLuaState::safe_require, this);
        _env.set_function("load", &SecureLuaState::safe_load, this);

        // Setting the global _environment
#if LUA_VERSION_NUM >= 502
        // Get _environment registry index
        lua_rawgeti(_lua, LUA_REGISTRYINDEX, _env.registry_index());

        // Set the global _environment
        lua_rawseti(_lua, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
#else
        // Get main thread
        int is_main = lua_pushthread(lua);
        assert(is_main);
        int thread = lua_gettop(lua);

        // Get _environment registry index
        lua_rawgeti(lua, LUA_REGISTRYINDEX, _env.registry_index());

        // Set the global _environment
        if (!lua_setf_env(lua, thread))
        {
            throw ModException(
                "Security: Unable to set _environment of the main Lua thread!");
        };
        lua_pop(lua, 1); // Pop thread
#endif
    }
} // namespace tilegame
