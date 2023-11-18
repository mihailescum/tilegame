#pragma once

#include <string>

namespace tilegame::components
{
    struct ScriptLoader
    {
        std::string path;
        bool loaded = false;

        ScriptLoader() {}
        ScriptLoader(const std::string path) : path(path) {}
    };
} // namespace tilegame::components
