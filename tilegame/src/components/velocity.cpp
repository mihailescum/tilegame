#include "velocity.hpp"

#include <entt/core/type_info.hpp>

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string Velocity::to_string() const
    {
        std::stringstream ss;
        ss << "Velocity";
        return ss.str();
    }

    void Velocity::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Velocity>();

        lua.new_usertype<Velocity>(
            "_Velocity",
            "type_id", &entt::type_hash<Velocity>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return Velocity(); },
                [](float velocity)
                { return Velocity(velocity); }),
            "velocity", &components::Velocity::velocity,
            sol::meta_function::to_string, &Velocity::to_string);
    }
} // namespace tilegame::components
