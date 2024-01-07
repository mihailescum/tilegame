#pragma once

#include <string>

#include "engine.hpp"

#include "system.hpp"

#include "components/transform.hpp"
#include "components/movement.hpp"
#include "components/tilelayer.hpp"
#include "components/collider.hpp"

namespace tilegame::systems
{
    class CollisionDetection : public System
    {
    private:
        void entity_tilelayer_detection(const components::Transform &entity_transform, const components::Collider &entity_collider, components::Movement &entity_movement, const components::TileLayer &tilelayer, const components::Transform &tilelayer_transform) const;
        bool circle_aabb_detection(const engine::Circle &a, const engine::Rectangle &b, const glm::vec2 &a_vel, glm::vec2 &contact_normal, float &contact_time) const;
        void circle_aabb_resolution(const engine::Circle &a, const engine::Rectangle &b, glm::vec2 &a_vel) const;
        bool aabb_aabb_detection(const engine::Rectangle &a, const engine::Rectangle &b, const glm::vec2 &a_vel, glm::vec2 &contact_normal, float &contact_time) const;
        void aabb_aabb_resolution(const engine::Rectangle &a, const engine::Rectangle &b, glm::vec2 &a_vel) const;
        bool circle_circle_detection(const engine::Circle &a, const engine::Circle &b, const glm::vec2 &a_vel, glm::vec2 &contact_normal, float &contact_time) const;
        void circle_circle_resolution(const engine::Circle &a, const engine::Circle &b, glm::vec2 &a_vel) const;

    public:
        CollisionDetection(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame