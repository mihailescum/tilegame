#pragma once

#include <string>

#include "engine.hpp"

#include "components/transform.hpp"
#include "components/movement.hpp"
#include "components/tilelayer.hpp"
#include "components/shape.hpp"
#include "components/collider.hpp"

namespace tilegame::systems
{
    /**
     * @brief Detects and resolves collisions between moving entities and tile layers.
     *
     * Operates on entities with Transform + Collider + Movement against every
     * entity carrying a Transform + TileLayer, testing the moving entity's
     * shape against each colliding tile of the layer and adjusting the
     * entity's Movement velocity in place to stop or deflect it. Must run
     * after the entity's intended velocity has been computed (e.g. by
     * MovementController) and before Movement applies that velocity to the
     * Transform.
     */
    class CollisionDetection : public engine::System
    {
    private:
        // Sweeps the entity's shape against every colliding tile in the layer, sorts the
        // hits by time of impact and resolves them (adjusting entity_movement.velocity)
        // in that order.
        void entity_tilelayer_detection(const components::Transform &entity_transform, const components::Collider &entity_collider, components::Movement &entity_movement, const components::TileLayer &tilelayer, const components::Transform &tilelayer_transform, const components::Shape &tilelayer_shape) const;

        // Continuous (swept) circle-vs-AABB test along a_vel; on hit, returns the surface
        // normal and normalized time of impact in [0,1). Not truly continuous: reduces to
        // a static nearest-point overlap test against the circle's projected position.
        bool circle_aabb_detection(const engine::Circle &a, const engine::Rectangle &b, const glm::vec2 &a_vel, glm::vec2 &contact_normal, float &contact_time) const;
        void circle_aabb_resolution(const engine::Circle &a, const engine::Rectangle &b, glm::vec2 &a_vel) const;

        // Continuous (swept) AABB-vs-AABB ray test, ported from javidx9's "Arbitrary
        // Rectangle Collision Detection & Resolution" (OLC-3 licensed, see .cpp).
        bool aabb_aabb_detection(const engine::Rectangle &a, const engine::Rectangle &b, const glm::vec2 &a_vel, glm::vec2 &contact_normal, float &contact_time) const;
        void aabb_aabb_resolution(const engine::Rectangle &a, const engine::Rectangle &b, glm::vec2 &a_vel) const;

        // Continuous circle-vs-circle test solving for the time of impact along a_vel.
        bool circle_circle_detection(const engine::Circle &a, const engine::Circle &b, const glm::vec2 &a_vel, glm::vec2 &contact_normal, float &contact_time) const;
        // Cancels only the velocity component along the contact normal, preserving the
        // tangential component so the entity slides along the other circle's surface.
        void circle_circle_resolution(const engine::Circle &a, const engine::Circle &b, glm::vec2 &a_vel) const;

    public:
        CollisionDetection(engine::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame