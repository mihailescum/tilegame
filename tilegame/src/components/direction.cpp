#include "direction.hpp"

#include <entt/core/type_info.hpp>

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string Direction::to_string() const
    {
        std::stringstream ss;
        ss << "Direction";
        return ss.str();
    }

    void Direction::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Direction>();

        lua.new_usertype<Direction>(
            "_Direction",
            "type_id", &entt::type_hash<Direction>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return Direction(); },
                [](const glm::vec2 &direction)
                { return Direction(direction); }),
            "direction", &Direction::direction,
            sol::meta_function::to_string, &Direction::to_string);
    }
} // namespace tilegame::components
