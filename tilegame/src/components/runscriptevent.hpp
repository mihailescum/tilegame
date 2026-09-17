#pragma once

#include <string>
#include <vector>

#include "sol/sol.hpp"

namespace tilegame::components
{
    // ---- Raised via System::raise_event() by the `_run_script` Lua binding (see
    // Script::run_script()), immediately delivered to systems::Script's own
    // EventListener<RunScriptEvent> (registered on a dedicated control entity in
    // Script::initialize()), which loads and runs the Lua file at `path` synchronously,
    // forwarding `arguments` to it as varargs (`...`) - see Script::execute_script(). `arguments`
    // may be empty; the script's return value, if any, is discarded either way. Not exposed to
    // Lua as a usertype - scripts raise it through `_run_script` directly, no Lua code ever
    // needs to construct or listen for it - but carries EVENT_TYPE like any other event since
    // raise_event() needs it regardless.
    struct RunScriptEvent
    {
        inline static const std::string EVENT_TYPE = "RUN_SCRIPT_EVENT";

        std::string path;
        std::vector<sol::object> arguments;
    };
} // namespace tilegame::components
