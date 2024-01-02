#include "movement.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "components/movement.hpp"
#include "components/target.hpp"
#include "components/speed.hpp"
#include "components/transform.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    Movement::Movement(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Movement::initialize()
    {
    }

    void Movement::update(const engine::GameTime &update_time)
    {
        apply_movement(update_time);
        check_target_reached();
    }

    void Movement::end_update()
    {
        _registry.clear<components::TargetReachedEvent>();
    }

    void Movement::apply_movement(const engine::GameTime &update_time)
    {
        auto view = _registry.view<const components::Movement>(entt::exclude<components::Inactive>);

        for (auto &&[entity, movement] : view.each())
        {
            if (glm::length2(movement.velocity) > 10e-8)
            {
                _registry.patch<components::Transform>(entity, [movement](auto &transform)
                                                       { transform.position += movement.velocity; });
            }
        }
    }

    void Movement::check_target_reached()
    {
        auto view = _registry.view<components::Transform, const components::Target>(entt::exclude<components::Inactive>);

        for (auto &&[entity, transform, target] : view.each())
        {
            // If we almost hit the target, we clamp the position to the target
            if (glm::length2(transform.position - target()) < 1e-8)
            {
                transform.position = target();
                _registry.patch<components::Transform>(entity);
                _registry.erase<components::Target, components::Movement, components::Speed>(entity);
                _registry.emplace<components::TargetReachedEvent>(entity);
            }
        }

        raise_events<components::TargetReachedEvent>();
    }
} // namespace tilegame::systems
