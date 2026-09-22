#include "movement.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "components/movement.hpp"
#include "components/target.hpp"
#include "components/speed.hpp"
#include "components/transform.hpp"
#include "components/inactive.hpp"
#include "components/currentmap.hpp"
#include "systems/world.hpp"

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
        update_current_map();
        check_target_reached();
    }

    void Movement::apply_movement(const engine::GameTime &update_time) const
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

    void Movement::check_target_reached() const
    {
        auto view = _registry.view<components::Transform, const components::Target>(entt::exclude<components::Inactive>);

        for (auto &&[entity, transform, target] : view.each())
        {
            // If we almost hit the target, we clamp the position to the target
            if (glm::length2(transform.position - target()) < 1e-8)
            {
                // Copied before erase() below, since `target` is a reference into the Target
                // component's storage and would dangle once it's erased.
                const glm::vec2 reached_target = target();

                transform.position = reached_target;
                _registry.patch<components::Transform>(entity);
                _registry.erase<components::Target, components::Movement, components::Speed>(entity);

                // Raised immediately: no current TargetReachedEvent listener adds/removes a
                // Target/Transform (see System::raise_event()'s caution), so this is safe
                // mid-iteration.
                raise_event<components::TargetReachedEvent>(entity, entt::null, reached_target);
            }
        }
    }

    void Movement::update_current_map() const
    {
        const auto view = _registry.view<const components::Movement, const components::Transform>(entt::exclude<components::Inactive>);
        for (auto &&[entity, movement, transform] : view.each())
        {
            if (!movement.track_current_map)
                continue;

            const entt::entity new_map = systems::World::map_at(_registry, transform.position);
            const entt::entity old_map = _registry.all_of<components::CurrentMap>(entity) ? _registry.get<components::CurrentMap>(entity)() : entt::null;

            if (new_map != old_map)
            {
                // Raised immediately: no current MapLeftEvent/MapEnteredEvent listener
                // adds/removes a Movement/Transform (see System::raise_event()'s caution), so
                // this is safe mid-iteration.
                // target = the map entity, so a listener can scope itself to one specific map
                // (e.g. content/scripts/map1.lua) in addition to/instead of the entity that moved.
                if (old_map != entt::null)
                {
                    raise_event<components::MapLeftEvent>(entity, old_map);
                }

                if (new_map == entt::null)
                {
                    _registry.erase<components::CurrentMap>(entity);
                }
                else
                {
                    raise_event<components::MapEnteredEvent>(entity, new_map);
                    _registry.emplace_or_replace<components::CurrentMap>(entity, new_map);
                }
            }
        }
    }
} // namespace tilegame::systems
