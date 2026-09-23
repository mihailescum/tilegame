#pragma once

#include "engine.hpp"

namespace tilegame::systems
{
    /**
     * @brief Keeps every entity's Facing pointing the way it last moved.
     *
     * Each frame, for every entity with both Direction and Facing, copies Direction into Facing
     * whenever Direction is non-zero. Direction itself goes to zero at rest (so
     * MovementController stops the entity), but Facing needs to keep pointing the last heading so
     * systems that care which way a stationary entity faces (systems::Interaction,
     * systems::SpriteOrientation) have something meaningful to read. Must run after whatever
     * writes Direction that frame - systems::Player for the player, systems::MovementController
     * for Target-driven entities - see WorldScene::update().
     */
    class Facing : public engine::System
    {
    public:
        Facing(engine::Scene &scene, entt::registry &registry);

        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame
