#include "movement.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "components/movement.hpp"
#include "components/transform.hpp"

namespace tilegame::systems
{
    MovementSystem::MovementSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void MovementSystem::update(const engine::GameTime &update_time)
    {
        auto view = _registry.view<tilegame::components::Movement>();

        for (auto &&[entity, movement] : view.each())
        {
            glm::vec2 movement_vector(0.0);
            if (movement.direction & tilegame::components::Movement::Left)
            {
                movement_vector.x -= 1.0;
            }
            if (movement.direction & tilegame::components::Movement::Right)
            {
                movement_vector.x += 1.0;
            }
            if (movement.direction & tilegame::components::Movement::Up)
            {
                movement_vector.y -= 1.0;
            }
            if (movement.direction & tilegame::components::Movement::Down)
            {
                movement_vector.y += 1.0;
            }

            if (glm::length2(movement_vector) > 10e-8)
            {
                movement_vector = update_time.elapsed_time * movement.speed * glm::normalize(movement_vector);
                _registry.patch<tilegame::components::Transform>(entity, [=](auto &transform)
                                                                 { transform.position_local += movement_vector; });
            }
        }
    }
} // namespace tilegame::systems
