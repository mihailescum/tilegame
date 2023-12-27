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
    MovementSystem::MovementSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void MovementSystem::initialize()
    {
        _registry.on_construct<components::Target>().connect<&MovementSystem::add_movement_component>(this);
    }

    void MovementSystem::add_movement_component(entt::registry &registry, entt::entity entity)
    {
        registry.emplace_or_replace<components::Movement>(entity, glm::vec2(0.0), 0.0);
    }

    void MovementSystem::update(const engine::GameTime &update_time)
    {
        update_move_to_target(update_time);

        apply_movement(update_time);
    }

    void MovementSystem::update_move_to_target(const engine::GameTime &update_time)
    {
        auto view = _registry.view<components::Transform, components::Target, components::Velocity, components::Movement>(entt::exclude<components::Inactive>);

        for (auto &&[entity, transform, target, velocity, movement] : view.each())
        {
            // Fix here if we want to use local or global position
            auto &position = transform.position_local;
            // If we almost reached the target, up to epsilon, we clamp the position to the target
            const auto direction = target() - position;
            if (glm::length2(direction) < 1e-6)
            {
                position = target();
                _registry.erase<components::Target, components::Movement>(entity);
                // TODO possibly fire some event that the target is reached
            }
            else
            {
                movement.direction = glm::normalize(direction);
                movement.speed = velocity();
            }
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
