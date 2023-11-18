#include "animation.hpp"

#include "components/animation.hpp"
#include "components/sprite.hpp"

namespace tilegame::systems
{
    AnimationSystem::AnimationSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void AnimationSystem::initialize()
    {
    }

    void AnimationSystem::update(const engine::GameTime &update_time)
    {
        const auto animation_view = _registry.view<tilegame::components::Animation>();
        const auto sprite_view = _registry.view<tilegame::components::Animation, tilegame::components::Sprite>();

        for (auto &&[entity, animation] : animation_view.each())
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

                if (sprite_view.contains(entity))
                {
                    _registry.patch<tilegame::components::Sprite>(entity, [=](auto &sprite)
                                                                  { sprite.source_rect = animation.frames[animation.current_frame_idx].source_rect; });
                }
            }
        }
    }
} // namespace tilegame::systems
