#include "world.hpp"

#include "components/map.hpp"
#include "components/transform.hpp"
#include "components/shape.hpp"

namespace tilegame::systems
{
    glm::vec2 World::to_global(const entt::registry &registry, entt::entity map, const glm::vec2 &relative_position)
    {
        return registry.get<const components::Transform>(map).position + relative_position;
    }

    entt::entity World::map_at(const entt::registry &registry, const glm::vec2 &global_position)
    {
        const auto view = registry.view<const components::Map, const components::Transform, const components::Shape>();
        for (auto &&[entity, transform, shape] : view.each())
        {
            const auto *dimensions = std::get_if<engine::Point>(&shape());
            if (dimensions && engine::Rectangle(transform.position, dimensions->position).intersects(engine::Point(global_position)))
            {
                return entity;
            }
        }

        return entt::null;
    }
} // namespace tilegame::systems
