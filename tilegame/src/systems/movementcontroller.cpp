#include "movementcontroller.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "components/movement.hpp"
#include "components/target.hpp"
#include "components/speed.hpp"
#include "components/direction.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    MovementController::MovementController(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void MovementController::initialize()
    {
        _registry.on_construct<components::Target>().connect<&MovementController::add_target_component>(this);
    }

    void MovementController::add_target_component(entt::registry &registry, entt::entity entity)
    {
        const auto &[target, transform] = registry.get<const components::Target, const components::Transform>(entity);
        registry.emplace_or_replace<components::Movement>(entity);
        registry.emplace_or_replace<components::Direction>(entity);

        if (!target.start)
        {
            registry.patch<components::Target>(entity,
                                               [transform](auto &target_component)
                                               {
                                                   target_component.start = transform.position;
                                               });
        }
    }

    void MovementController::update(const engine::GameTime &update_time)
    {
        update_direction_from_target(update_time);
        update_movement(update_time);
    }

    void MovementController::update_direction_from_target(const engine::GameTime &update_time)
    {
        auto view = _registry.view<components::Direction, const components::Target, const components::Transform, const components::Speed>(entt::exclude<components::Inactive>);

        for (auto &&[entity, direction, target, transform, speed] : view.each())
        {
            glm::vec2 to_target = target() - transform.position;
            direction() = glm::normalize(to_target);

            float distance_covered = update_time.elapsed_time * speed();
            if (glm::length2(to_target) < distance_covered * distance_covered)
            {
                direction() *= glm::length(to_target) / distance_covered;
            }

            _registry.patch<components::Direction>(entity);
        }
    }

    void MovementController::update_movement(const engine::GameTime &update_time)
    {
        auto view = _registry.view<components::Movement, const components::Direction, const components::Speed>(entt::exclude<components::Inactive>);

        for (auto &&[entity, movement, direction, speed] : view.each())
        {
            movement.velocity = direction() * speed() * update_time.elapsed_time;
            _registry.patch<components::Movement>(entity);
        }
    }

    void MovementController::end_update()
    {
    }
} // namespace tilegame::systems
