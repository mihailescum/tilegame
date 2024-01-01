#include "speed.hpp"

#include <entt/core/type_info.hpp>

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string Speed::to_string() const
    {
        std::stringstream ss;
        ss << "Speed";
        return ss.str();
    }

    void Speed::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Speed>();

        lua.new_usertype<Speed>(
            "_Speed",
            "type_id", &entt::type_hash<Speed>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return Speed(); },
                [](float velocity)
                { return Speed(velocity); }),
            "speed", &components::Speed::speed,
            sol::meta_function::to_string, &Speed::to_string);
    }
} // namespace tilegame::components
