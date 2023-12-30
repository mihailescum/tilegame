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

        sol::object safe_require(const std::string &name);
        void safe_write(const std::string &prompt);
        std::string safe_read(const std::string &prompt);
        std::tuple<sol::object, sol::object> safe_load(const std::string &str, const std::string &chunkname);

    public:
        sol::state &
        operator()() { return _lua; }
        void open_libraries();
    };
} // namespace tilegame
