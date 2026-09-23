#pragma once

#include <string>

#include "engine.hpp"

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
     * and immediately raising a TargetReachedEvent (via System::raise_event())
     * for any Lua/native event listeners. Also tracks which map each movable
     * entity (one with a Movement component) is currently on (CurrentMap),
     * immediately raising MapLeftEvent/MapEnteredEvent when it changes.
     */
    class Movement : public engine::System
    {
    private:
        void apply_movement(const engine::GameTime &update_time) const;
        void check_target_reached() const;
        void update_current_map() const;

    public:
        Movement(engine::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame