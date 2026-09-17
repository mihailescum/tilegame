#include "shape.hpp"

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    void Shape::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Shape>();

        lua.new_usertype<Shape>(
            "_Shape",
            "type_id", &entt::type_hash<Shape>::value,
            sol::call_constructor,
            sol::factories(
                [](const engine::Rectangle &rectangle)
                { return Shape(rectangle); }));
    }
} // namespace tilegame::components
