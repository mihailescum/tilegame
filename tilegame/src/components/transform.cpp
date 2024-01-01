#include "transform.hpp"

#include <entt/core/type_info.hpp>

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string Transform::to_string() const
    {
        std::stringstream ss;
        ss << "Transform";
        return ss.str();
    }

    void Transform::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Transform>();

        lua.new_usertype<Transform>(
            "_Transform",
            "type_id", &entt::type_hash<Transform>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return Transform(); },
                [](float x, float y)
                { return Transform{glm::vec2(x, y)}; },
                [](const glm::vec2 &position)
                { return Transform{position}; }),
            "position", &components::Transform::position,
            sol::meta_function::to_string, &Transform::to_string);
    }
} // namespace tilegame::components
