#include "spriteorientation.hpp"

#include "components/facing.hpp"
#include "components/spriteorientation.hpp"
#include "components/animation.hpp"
#include "components/sprite.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    SpriteOrientation::SpriteOrientation(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void SpriteOrientation::update(const engine::GameTime &update_time)
    {
        auto view = _registry.view<const components::Facing, components::SpriteOrientation, components::Animation>(entt::exclude<components::Inactive>);
        const auto sprite_entities = _registry.view<const components::Sprite>();

        for (auto &&[entity, facing, orientation, animation] : view.each())
        {
            const auto direction = components::SpriteOrientation::direction_from_heading(facing());
            if (direction == orientation.current_direction)
                continue;

            const auto state_string = components::SpriteOrientation::direction_prefix(direction) + "_" + orientation.action;
            const auto &state = orientation.sprite->has_state(state_string) ? (*orientation.sprite)[state_string] : (*orientation.sprite)["down_walking"];

            orientation.current_direction = direction;
            animation.frames = state.frames;
            animation.current_frame_idx = 0;
            animation.clock = 0.0f;

            if (sprite_entities.contains(entity))
            {
                _registry.patch<components::Sprite>(entity, [&animation](auto &sprite)
                                                    { sprite.source_rect = animation.get_current_frame().source_rect; });
            }
        }
    }
} // namespace tilegame::systems
