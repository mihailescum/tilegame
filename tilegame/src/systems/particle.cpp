#include "particle.hpp"

#include <algorithm>

#include <glm/gtx/rotate_vector.hpp>

#include "helper.hpp"
#include "components/particleemitter.hpp"
#include "components/particlepool.hpp"
#include "components/transform.hpp"
#include "components/shape.hpp"
#include "components/scenenode.hpp"
#include "components/inactive.hpp"
#include "components/movement.hpp"
#include "components/particle.hpp"

namespace tilegame::systems
{
    ParticleSystem::ParticleSystem(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    void ParticleSystem::initialize()
    {
        auto emitter1 = _registry.create();
        _registry.emplace<components::Transform>(emitter1, glm::vec2(150, 150), glm::vec2(0.0));
        _registry.emplace<components::Shape>(emitter1, components::Shape(std::make_unique<engine::Point>(engine::Point::ZERO)));
        _registry.emplace<components::ParticleEmitter>(emitter1,
                                                       components::ParticleEmitter(
                                                           2,
                                                           glm::vec2(1.0, 0.0),
                                                           glm::pi<float>() / 8.0,
                                                           10.0, 20.0,
                                                           1.0, 2.0,
                                                           0.9, 1.1,
                                                           engine::Color::WHITE));
        _registry.emplace<components::ParticlePool>(emitter1, components::ParticlePool());

        const tilegame::SceneGraphData emitter1_scenedata(emitter1);
        tilegame::SceneGraphNode &emitter1_scenenode = _scene.scene_graph_root().add_child(emitter1_scenedata);
        _registry.emplace<components::SceneNode>(emitter1, &emitter1_scenenode);
    }

    void ParticleSystem::load_content()
    {
        const auto &particles_texture = *_scene.game().resource_manager().load_resource<engine::Texture2D>("particles", "content/textures/particles.png");
    }

    void ParticleSystem::update(const engine::GameTime &update_time)
    {
        update_particles(update_time);
        emit_particles(update_time);
    }

    void ParticleSystem::update_particles(const engine::GameTime &update_time)
    {
        const auto emitter_view = _registry.view<tilegame::components::ParticlePool>(entt::exclude<tilegame::components::Inactive>);
        auto particles_view = _registry.view<tilegame::components::Particle>(entt::exclude<tilegame::components::Inactive>);

        for (const auto &&[entity, pool] : emitter_view.each())
        {
            for (size_t i = 0; i < pool.first_dead_particle; ++i)
            {
                auto &particle = particles_view.get<tilegame::components::Particle>(pool.container[i]);
                particle.lifetime_left -= update_time.elapsed_time;
                if (particle.lifetime_left <= 0)
                {
                    kill_particle(entity, i, pool);
                    --i;
                }
            }
        }
    }

    void ParticleSystem::emit_particles(const engine::GameTime &update_time)
    {
        const auto emitter_view = _registry.view<tilegame::components::ParticleEmitter, tilegame::components::ParticlePool, tilegame::components::Transform, tilegame::components::Shape>(entt::exclude<tilegame::components::Inactive>);

        for (const auto &&[entity, emitter, pool, transform, shape] : emitter_view.each())
        {
            emitter.rate_clock += update_time.elapsed_time;
            int num_new_particles = static_cast<int>(emitter.rate * emitter.rate_clock);
            float time_to_produce_new_particles = num_new_particles / static_cast<float>(emitter.rate);
            emitter.rate_clock -= time_to_produce_new_particles;

            if (num_new_particles > 0)
            {
                if (grow_pool(entity, pool, num_new_particles))
                {
                    // TODO trigger pool resize event
                }

                for (int i = 0; i < num_new_particles; ++i)
                {
                    const auto new_particle = emit_particle(emitter, pool);
                }
            }
        }
    }

    entt::entity ParticleSystem::emit_particle(const tilegame::components::ParticleEmitter &emitter, tilegame::components::ParticlePool &pool)
    {
        float lifetime = get_random(emitter.lifetime.x, emitter.lifetime.y);
        float scale = get_random(emitter.scale.x, emitter.scale.y);

        float speed = get_random(emitter.speed.x, emitter.speed.y);
        float angle = get_random(-emitter.spread_angle, emitter.spread_angle);
        glm::vec2 direction = speed * glm::rotate(emitter.spread_direction, angle);

        glm::vec2 position(0.0);

        // Get entity and update components
        const auto new_particle = pool.container[pool.first_dead_particle++];

        _registry.remove<tilegame::components::Inactive>(new_particle);
        _registry.patch<tilegame::components::Movement>(new_particle, [speed, &direction](auto &movement)
                                                        { movement.direction = direction; movement.speed = speed; });
        _registry.patch<tilegame::components::Particle>(new_particle,
                                                        [lifetime, scale](auto &particle)
                                                        {
                                                            particle.lifetime_left = lifetime;
                                                            particle.scale = scale;
                                                        });
        _registry.patch<tilegame::components::Transform>(new_particle, [&position](auto &transform)
                                                         { transform.position_local = position; });

        return new_particle;
    }

    void ParticleSystem::kill_particle(const entt::entity particle_entity, size_t index, tilegame::components::ParticlePool &pool)
    {
        _registry.emplace<tilegame::components::Inactive>(particle_entity);
        std::iter_swap(pool.container.begin() + index, pool.container.begin() + pool.first_dead_particle - 1);
        --pool.first_dead_particle;
    }

    bool ParticleSystem::grow_pool(const entt::entity emitter_entity, tilegame::components::ParticlePool &pool, int num_new_particles)
    {
        std::size_t old_size = pool.container.size();
        if (pool.first_dead_particle >= old_size * pool.growth_threshold || pool.container.size() - pool.first_dead_particle < num_new_particles)
        {
            std::size_t new_size = old_size * pool.growth_factor;
            new_size = std::max(new_size, pool.first_dead_particle + num_new_particles);

            pool.container.resize(new_size);

            auto emitter_scenenode = _registry.get<components::SceneNode>(emitter_entity).node;

            for (std::size_t i = old_size; i < new_size; ++i)
            {
                const auto new_particle = _registry.create();
                pool.container[i] = new_particle;

                _registry.emplace<tilegame::components::Inactive>(new_particle);
                _registry.emplace<tilegame::components::Movement>(new_particle);
                _registry.emplace<tilegame::components::Particle>(new_particle);
                _registry.emplace<tilegame::components::Transform>(new_particle);

                const tilegame::SceneGraphData scenedata(new_particle);
                tilegame::SceneGraphNode &scenenode = emitter_scenenode->add_child(scenedata);
                _registry.emplace<components::SceneNode>(new_particle, &scenenode);
            }
            return true;
        }
        return false;
    }
} // namespace tilegame::systems
