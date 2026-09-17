#include "sprite.hpp"

#include "entt/entt.hpp"

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    void Sprite::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Sprite>();

        lua.new_usertype<Sprite>(
            "_Sprite",
            "type_id", &entt::type_hash<Sprite>::value,
            sol::call_constructor,
            sol::factories(
                [](const engine::Texture2D *texture, const engine::Texture2D *luminosity_texture, const engine::Rectangle &source_rect)
                { return Sprite{engine::Texture2DContainer<2>{texture, luminosity_texture}, source_rect}; }));
    }
} // namespace tilegame::components
