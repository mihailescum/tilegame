#include "inactive.hpp"

#include <entt/core/type_info.hpp>

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string Inactive::to_string() const
    {
        std::stringstream ss;
        ss << "Inactive";
        return ss.str();
    }

    void Inactive::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Inactive>();

        lua.new_usertype<Inactive>(
            "_Inactive",
            "type_id", &entt::type_hash<Inactive>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return Inactive{}; }),
            sol::meta_function::to_string, &Inactive::to_string);
    }
} // namespace tilegame::components
