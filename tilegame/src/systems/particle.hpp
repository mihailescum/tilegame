#pragma once

#include <entt/entity/registry.hpp>

#include "system.hpp"
#include "components/particleemitter.hpp"
#include "components/particlepool.hpp"

namespace tilegame::systems
{
    class ParticleSystem : public System
    {
    private:
        void update_particles(const engine::GameTime &update_time);
        void emit_particles(const engine::GameTime &update_time);

        entt::entity emit_particle(const components::ParticleEmitter &emitter, components::ParticlePool &pool);
        void kill_particle(const entt::entity particle_entity, size_t index, components::ParticlePool &pool);
        bool grow_pool(const entt::entity emitter_entity, components::ParticlePool &pool, int num_new_particles);

    public:
        ParticleSystem(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
