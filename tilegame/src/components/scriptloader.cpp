#include "scriptloader.hpp"

#include <entt/core/type_info.hpp>

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string ScriptLoader::to_string() const
    {
        std::stringstream ss;
        ss << "ScriptLoader";
        return ss.str();
    }

    void ScriptLoader::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<ScriptLoader>();

        lua.new_usertype<ScriptLoader>(
            "_ScriptLoader",
            "type_id", &entt::type_hash<ScriptLoader>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return ScriptLoader(); },
                [](const std::string path)
                { return ScriptLoader(path); }),
            "path", &components::ScriptLoader::path,
            sol::meta_function::to_string, &ScriptLoader::to_string);
    }
} // namespace tilegame::components
