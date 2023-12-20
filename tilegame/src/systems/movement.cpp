#include "movement.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "components/movement.hpp"
#include "components/movetotarget.hpp"
#include "components/velocity.hpp"
#include "components/transform.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    MovementSystem::MovementSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void MovementSystem::update(const engine::GameTime &update_time)
    {
        update_move_to_target(update_time);

        apply_movement(update_time);
    }

    void MovementSystem::update_move_to_target(const engine::GameTime &update_time)
    {
        auto view = _registry.view<components::MoveToTarget, components::Velocity, components::Movement>(entt::exclude<components::Inactive>);

        for (auto &&[entity, move_target, velocity, movement] : view.each())
        {
            movement.direction = glm::normalize(move_target.end - move_target.start);
            // if (movement.direction * update_time.elapsed_time *)
        }
    }

    void MovementSystem::apply_movement(const engine::GameTime &update_time)
    {
        auto view = _registry.view<components::Movement>(entt::exclude<components::Inactive>);

        for (auto &&[entity, movement] : view.each())
        {
            if (glm::length2(movement.direction) > 10e-8)
            {
                _registry.patch<components::Transform>(entity, [movement, update_time](auto &transform)
                                                       { transform.position_local += update_time.elapsed_time * movement.speed * movement.direction; });
            }
        }
    }
} // namespace tilegame::systems
