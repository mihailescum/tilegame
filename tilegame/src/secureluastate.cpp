#include "secureluastate.hpp"

#include <iostream>
#include <stdexcept>

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
            throw std::runtime_error("Unsupported module: " + name + "!");
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

    int SecureLuaState::location_prefixed_exception_handler(lua_State *L, sol::optional<const std::exception &>, sol::string_view what)
    {
        luaL_where(L, 1);
        lua_pushlstring(L, what.data(), what.size());
        lua_concat(L, 2);
        return 1;
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

        // Must come after the globals swap above, not before: set_exception_handler() stores
        // the handler via lua_setglobal(), which writes into whatever table LUA_RIDX_GLOBALS
        // currently points to - setting it any earlier would register it on the original globals
        // table this function is about to replace, leaving it unreachable once _env takes over.
        _lua.set_exception_handler(&SecureLuaState::location_prefixed_exception_handler);

        // sol2 installs its own protected-call message handler (default_traceback_error_handler,
        // which uses luaL_traceback() to walk the whole call stack, not just the current frame -
        // the only way to get a call site for an error raised inside a Lua stdlib C function like
        // ipairs/pairs, which have no Lua source line of their own to report) the moment a
        // sol::state is constructed - i.e. before this method ever runs, let alone the globals
        // swap above, via the exact same lua_setglobal()-based mechanism as the exception handler
        // just above. So it's been just as unreachable as that was - reinstall it here too, now
        // that it'll actually land on the table _run_script/execute_script's protected calls read.
        sol::protected_function::set_default_handler(
            sol::object(_lua.lua_state(), sol::in_place,
                        sol::c_call<decltype(&sol::default_traceback_error_handler), &sol::default_traceback_error_handler>));
    }
} // namespace tilegame
