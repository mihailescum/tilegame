#include "ordering.hpp"

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    void Ordering::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Ordering>();

        lua.new_usertype<Ordering>(
            "_Ordering",
            "type_id", &entt::type_hash<Ordering>::value,
            sol::call_constructor,
            sol::factories(
                [](float z)
                { return Ordering{z}; }),
            "z", &Ordering::z);
    }
} // namespace tilegame::components
