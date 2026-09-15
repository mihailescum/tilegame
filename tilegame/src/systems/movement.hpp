#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Applies computed velocity to entity positions.
     *
     * Operates on entities with a Movement component, adding its velocity to
     * their Transform position each frame. Must run after MovementController
     * and CollisionDetection have produced/adjusted that velocity. Also
     * detects entities with a Target that have (almost) arrived, snapping
     * them to the target, dropping their Target/Movement/Speed components,
     * and raising a TargetReachedEvent for any Lua event listeners.
     */
    class Movement : public System
    {
    private:
        void apply_movement(const engine::GameTime &update_time) const;
        void check_target_reached() const;

    public:
        Movement(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
        void end_update();
    };
} // namespace tilegame