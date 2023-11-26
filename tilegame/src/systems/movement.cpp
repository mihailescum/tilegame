#include "movement.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "components/movement.hpp"
#include "components/transform.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    MovementSystem::MovementSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void MovementSystem::update(const engine::GameTime &update_time)
    {
        auto view = _registry.view<components::Movement>(entt::exclude<components::Inactive>);

        for (auto &&[entity, movement] : view.each())
        {
            if (glm::length2(movement.direction) > 10e-8)
            {
                _registry.patch<components::Transform>(entity, [=](auto &transform)
                                                       { transform.position_local += update_time.elapsed_time * movement.speed * movement.direction; });
            }
        }
    }
} // namespace tilegame::systems
