#include "depth.hpp"

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    void Depth::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Depth>();

        lua.new_usertype<Depth>(
            "_Depth",
            "type_id", &entt::type_hash<Depth>::value,
            sol::call_constructor,
            sol::factories(
                [](float z)
                { return Depth{z}; }),
            "z", &Depth::z);
    }
} // namespace tilegame::components
