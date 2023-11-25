#pragma once

#include "system.hpp"
#include <entt/entity/registry.hpp>

namespace tilegame::systems
{
    class ParticleSystem : public System
    {
    private:
        entt::entity create_particle(float lifetime, float scale, engine::Color color, glm::vec2 velocity);

    public:
        ParticleSystem(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
