#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Turns a Target destination into per-frame Movement velocity.
     *
     * Operates on entities with Direction + Target + Transform + Speed to
     * face them towards their target (clamping overshoot on the final frame),
     * then converts that Direction + Speed into a Movement velocity. Must run
     * before CollisionDetection/Movement so there is a velocity for those
     * systems to resolve/apply.
     */
    class MovementController : public System
    {
    private:
        void update_movement(const engine::GameTime &update_time);
        void update_direction_from_target(const engine::GameTime &update_time);

        // Hooked to entt::registry::on_construct<Target> in initialize(): whenever a Target
        // is added to an entity, auto-attaches the Movement/Direction components it needs
        // and, if the target wasn't given a start position, seeds it from the entity's
        // current Transform.
        void add_target_component(entt::registry &registry, entt::entity entity);

    public:
        MovementController(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
        void end_update();
    };
} // namespace tilegame