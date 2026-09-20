#include "facing.hpp"

#include <glm/gtx/norm.hpp>

#include "components/direction.hpp"
#include "components/facing.hpp"
#include "components/inactive.hpp"

namespace tilegame::systems
{
    Facing::Facing(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void Facing::update(const engine::GameTime &update_time)
    {
        auto view = _registry.view<const components::Direction, components::Facing>(entt::exclude<components::Inactive>);

        for (auto &&[entity, direction, facing] : view.each())
        {
            if (glm::length2(direction()) > 1e-10)
            {
                facing.direction = direction();
            }
        }
    }
} // namespace tilegame::systems
