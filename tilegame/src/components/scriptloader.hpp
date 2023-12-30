#pragma once

#include <string>

#include "sol/sol.hpp"

namespace tilegame::components
{
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
