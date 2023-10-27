#include "movementsystem.hpp"

#include <glm/glm.hpp>

#include "components/movement.hpp"
#include "components/localtransform.hpp"

namespace tilegame::systems
{
    MovementSystem::MovementSystem(engine::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void MovementSystem::update(const engine::GameTime &update_time)
    {
        auto view = _registry.view<tilegame::components::Movement, tilegame::components::LocalTransform>();

        for (auto &&[entity, movement, transform] : view.each())
        {
            glm::vec3 movement_vector;
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

            movement_vector = update_time.elapsed_time * movement.speed * glm::normalize(movement_vector);
            transform.position *= movement_vector;
        }
    }
} // namespace tilegame::systems
