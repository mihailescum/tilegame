#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace tilegame
{
    // See https://blog.rubenwardy.com/2020/07/26/sol3-script-sandbox/ for the setup of our sandbox
    // Here http://lua-users.org/wiki/SandBoxes for a reference of safe functions
    /**
     * @brief Sandboxed sol3 Lua state used to run untrusted-by-design game/story scripts.
     *
     * Wraps a plain sol::state with a restricted global environment: only
     * whitelisted base functions, stdlib modules and os functions are
     * exposed, `require`/`load` are replaced with safe versions (source-only,
     * no bytecode loading), so scripts loaded by systems::Script cannot
     * escape the sandbox to access unsafe host APIs.
     */
    class SecureLuaState
    {
    private:
        sol::state _lua;
        sol::environment _env;

        const std::vector<std::string> _whitelisted_functions = {
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
            "getmetatable",
        };

        const std::unordered_map<std::string, sol::lib> _whitelisted_modules{
            {"coroutine", sol::lib::coroutine},
            {"table", sol::lib::table},
            {"string", sol::lib::string},
            {"math", sol::lib::math},
            {"debug", sol::lib::debug}};

        const std::vector<std::string> _whitelisted_os_functions = {
            "clock",
            "date",
            "time",
            "difftime",
            "exit",
        };

        // Sandbox replacement for `require`: only allows a fixed set of known-safe scripts
        // (currently "debugger" and "inspect"), loaded from embedded sources rather than
        // the filesystem.
        sol::object safe_require(const std::string &name);
        void safe_write(const std::string &prompt);
        std::string safe_read(const std::string &prompt);
        // Sandbox replacement for `load`: rejects precompiled bytecode (identified by Lua's
        // signature byte) so scripts can only load plain Lua source, then binds the
        // sandboxed environment onto the resulting function.
        std::tuple<sol::object, sol::object> safe_load(const std::string &str, const std::string &chunkname);

        // Registered with sol::state::set_exception_handler() in open_libraries(): sol2's own
        // default handler just pushes the raw C++ exception message with no location, unlike a
        // native Lua runtime error (which luaG_addinfo() always prefixes with "chunkname:line: ").
        // luaL_where() is the same call luaL_error() uses internally to get that prefix, so this
        // makes a C++ exception thrown from a Lua-bound function (e.g. _load_json failing) read
        // the same way a plain Lua error already does. Must be a plain function (not a member
        // function bound to `this`) since sol2 stores it as a raw function pointer.
        static int location_prefixed_exception_handler(lua_State *L, sol::optional<const std::exception &> maybe_exception, sol::string_view what);

    public:
        sol::state &
        operator()() { return _lua; }
        void open_libraries();
    };
} // namespace tilegame
