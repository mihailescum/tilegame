#include "pin.hpp"

#include <entt/core/type_info.hpp>

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string Pin::to_string() const
    {
        std::stringstream ss;
        ss << "Pin";
        return ss.str();
    }

    void Pin::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Pin>();

        lua.new_usertype<Pin>(
            "_Pin",
            "type_id", &entt::type_hash<Pin>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return Pin(); },
                [](entt::entity parent)
                { return Pin(parent); }),
            "parent", &components::Pin::parent,
            sol::meta_function::to_string, &Pin::to_string);
    }
} // namespace tilegame::components
