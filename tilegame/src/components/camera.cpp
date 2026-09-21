#include "camera.hpp"

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string Camera::to_string() const
    {
        std::stringstream ss;
        ss << "Camera";
        return ss.str();
    }

    void Camera::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Camera>();

        lua.new_usertype<Camera>(
            "_Camera",
            sol::no_constructor,
            "type_id", &entt::type_hash<Camera>::value,
            "scale", &Camera::scale,
            "visible_bounds", &Camera::visible_bounds,
            sol::meta_function::to_string, &Camera::to_string);
    }
} // namespace tilegame::components
