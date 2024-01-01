#include "movement.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "components/movement.hpp"
#include "components/target.hpp"
#include "components/velocity.hpp"
#include "components/transform.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    Movement::Movement(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Movement::initialize()
    {
        _registry.on_construct<components::Target>().connect<&Movement::add_movement_component>(this);
    }

    void Movement::add_movement_component(entt::registry &registry, entt::entity entity)
    {
        auto [target, transform] = registry.get<const components::Target, const components::Transform>(entity);
        const glm::vec2 direction = glm::normalize(target() - transform.position);
        registry.emplace_or_replace<components::Movement>(entity, direction);

        if (!target.start)
        {
            registry.patch<components::Target>(entity,
                                               [transform](auto &target_component)
                                               {
                                                   target_component.start = transform.position;
                                               });
        }
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
        auto view = _registry.view<const components::Movement, const components::Velocity>(entt::exclude<components::Inactive>);

        for (auto &&[entity, movement, velocity] : view.each())
        {
            if (glm::length2(movement.direction) > 10e-8)
            {
                _registry.patch<components::Transform>(entity, [movement, velocity, update_time](auto &transform)
                                                       { transform.position += update_time.elapsed_time * velocity() * movement.direction; });
            }
        }
    }

    void Movement::check_target_reached()
    {
        auto view = _registry.view<components::Transform, const components::Target, components::Movement>(entt::exclude<components::Inactive>);

        for (auto &&[entity, transform, target, movement] : view.each())
        {
            const auto direction_movement = movement.direction;
            const auto direction_to_target = target() - transform.position;
            // If we overshot the target, we clamp the position to the target
            if (glm::dot(direction_movement, direction_to_target) < 0)
            {
                transform.position = target();
                _registry.patch<components::Transform>(entity);
                _registry.erase<components::Target, components::Movement, components::Velocity>(entity);
                _registry.emplace<components::TargetReachedEvent>(entity);
            }
        }

        raise_events<components::TargetReachedEvent>();
    }
} // namespace tilegame::systems
