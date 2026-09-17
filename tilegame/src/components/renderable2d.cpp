#include "renderable2d.hpp"

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    void Renderable2D::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Renderable2D>();

        lua.new_usertype<Renderable2D>(
            "_Renderable2D",
            "type_id", &entt::type_hash<Renderable2D>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return Renderable2D{}; }));
    }
} // namespace tilegame::components
