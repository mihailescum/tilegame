#include "facing.hpp"

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string Facing::to_string() const
    {
        std::stringstream ss;
        ss << "Facing";
        return ss.str();
    }

    void Facing::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Facing>();

        lua.new_usertype<Facing>(
            "_Facing",
            "type_id", &entt::type_hash<Facing>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return Facing(); },
                [](const glm::vec2 &direction)
                { return Facing(direction); }),
            "direction", &Facing::direction,
            sol::meta_function::to_string, &Facing::to_string);
    }
} // namespace tilegame::components
