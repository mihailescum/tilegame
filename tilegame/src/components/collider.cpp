#include "collider.hpp"

#include <stdexcept>

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    void Collider::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Collider>();

        lua.new_usertype<Collider>(
            "_Collider",
            "type_id", &entt::type_hash<Collider>::value,
            sol::call_constructor,
            sol::factories(
                [](const sol::table &descriptor)
                { return Collider(make_shape(descriptor)); }));
    }

    engine::ShapeVariant Collider::make_shape(const sol::table &descriptor)
    {
        const std::string kind = descriptor["kind"];
        const glm::vec2 position = descriptor["position"];

        if (kind == "rectangle")
        {
            const glm::vec2 dimensions = descriptor["dimensions"];
            return engine::Rectangle(position, dimensions);
        }
        else if (kind == "circle")
        {
            const float radius = descriptor["radius"];
            return engine::Circle(position, radius);
        }
        else if (kind == "point")
        {
            return engine::Point(position);
        }
        else
        {
            throw std::runtime_error("Unknown collider shape kind");
        }
    }
} // namespace tilegame::components
