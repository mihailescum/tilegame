#pragma once

#include <string>

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Requests that a Lua script at `path` be loaded and run for this entity. The Script
     * system executes it on the next update, attaches the returned table as a LuaTable component,
     * and removes this component. Exposed to Lua as `_ScriptLoader`.
     */
    struct ScriptLoader
    {
        std::string path;
        bool loaded = false;

        ScriptLoader() {}
        ScriptLoader(const std::string path) : path(path) {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame::components
