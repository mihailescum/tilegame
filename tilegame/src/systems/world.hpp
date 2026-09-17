#pragma once

#include "entt/entt.hpp"

#include "engine.hpp"

namespace tilegame::systems
{
    /**
     * @brief Stateless world-space queries across map entities - the ECS-native replacement for
     * the former engine::tilemap::World resource. A map entity is tagged components::Map, with a
     * Transform (its position) and a Shape (its pixel-space bounds, relative to that Transform),
     * created once per map by content/scripts/maploader.lua. No instance state; called directly
     * as World::to_global()/World::map_at() from whichever system needs it (systems::Movement
     * for current-map tracking, systems::Script's `_to_global` binding).
     */
    struct World
    {
        /** @brief Converts a position relative to `map`'s own Transform into world space. */
        static glm::vec2 to_global(const entt::registry &registry, entt::entity map, const glm::vec2 &relative_position);

        /** @brief The map entity whose bounds (Transform + Shape) contain `global_position`, or entt::null if none match. */
        static entt::entity map_at(const entt::registry &registry, const glm::vec2 &global_position);
    };
} // namespace tilegame::systems
