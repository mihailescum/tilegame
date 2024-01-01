#include "animation.hpp"

#include "components/animation.hpp"
#include "components/sprite.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    Animation::Animation(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Animation::initialize()
    {
    }

    void Animation::update(const engine::GameTime &update_time)
    {
        const auto animation_entities = _registry.view<components::Animation>(entt::exclude<components::Inactive>);
        const auto sprite_entities = _registry.view<const components::Animation, const components::Sprite>(entt::exclude<components::Inactive>);

        for (auto &&[entity, animation] : animation_entities.each())
        {
            animation.clock += update_time.elapsed_time;
            auto &current_frame = animation.frames[animation.current_frame_idx];
            if (animation.clock >= current_frame.duration)
            {
                if (animation.current_frame_idx == animation.frames.size() - 1)
                {
                    animation.current_frame_idx = 0;
                }
                else
                {
                    animation.current_frame_idx++;
                }
                animation.clock -= current_frame.duration;

                if (sprite_entities.contains(entity))
                {
                    _registry.patch<components::Sprite>(entity, [=](auto &sprite)
                                                        { sprite.source_rect = animation.frames[animation.current_frame_idx].source_rect; });
                }
            }
        }
    }
} // namespace tilegame::systems
