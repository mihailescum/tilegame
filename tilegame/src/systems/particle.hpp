#pragma once

#include <glm/glm.hpp>
#include "entt/entt.hpp"

#include "system.hpp"
#include "components/particleemitter.hpp"
#include "components/particlepool.hpp"
#include "components/shape.hpp"

namespace tilegame::systems
{
    /**
     * @brief Emits and simulates pooled particle effects.
     *
     * Operates on emitter entities (ParticleEmitter + ParticlePool + Transform
     * + Shape), spawning new particles at the emitter's configured rate and
     * ageing/killing existing ones. Particle entities themselves (Particle +
     * Direction + Speed + Movement + Sprite + Transform) are never
     * created/destroyed at runtime: they are pre-allocated in the pool as
     * Inactive and toggled active/inactive as they are emitted/killed.
     */
    class Particle : public System
    {
    private:
        void update_particles(const engine::GameTime &update_time);
        void emit_particles(const engine::GameTime &update_time);
        glm::vec2 generate_random_position(const components::Shape &shape);

        entt::entity emit_particle(const components::ParticleEmitter &emitter, components::ParticlePool &pool, const components::Transform &emmiter_transform, const components::Shape &emitter_shape);
        // Recycles a particle back into the pool's dead region: marks it Inactive and swaps
        // it with the last live slot (pool.first_dead_particle), so live particles always
        // occupy container[0, first_dead_particle).
        void kill_particle(const entt::entity particle_entity, size_t index, components::ParticlePool &pool);
        // Grows the pool's backing storage (by growth_factor) when it is nearly exhausted or
        // too small for num_new_particles, pre-creating the new particle entities as Inactive.
        bool grow_pool(const entt::entity emitter_entity, components::ParticlePool &pool, int num_new_particles);

    public:
        Particle(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
