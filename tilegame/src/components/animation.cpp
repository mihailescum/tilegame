#include "animation.hpp"

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    void Animation::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Animation>();

        lua.new_usertype<Animation>(
            "_Animation",
            "type_id", &entt::type_hash<Animation>::value,
            sol::call_constructor,
            sol::factories(
                [](engine::graphics::Sprite &sprite_class, const std::string &state_name)
                { return Animation{0.0, 0, sprite_class[state_name].frames}; }));
    }
} // namespace tilegame::components
