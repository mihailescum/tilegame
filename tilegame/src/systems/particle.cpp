#include "particle.hpp"

#include <glm/gtx/rotate_vector.hpp>

#include "helper.hpp"
#include "components/particleemitter.hpp"
#include "components/transform.hpp"
#include "components/shape.hpp"
#include "components/scenenode.hpp"

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
                                                           10,
                                                           glm::vec2(1.0, 0.0),
                                                           glm::pi<float>() / 8.0,
                                                           10.0, 20.0,
                                                           10.0, 20.0,
                                                           0.9, 1.1,
                                                           engine::Color::WHITE));

        const tilegame::SceneGraphData emitter1_scenedata(emitter1);
        tilegame::SceneGraphNode &emitter1_scenenode = _scene.scene_graph_root().add_child(emitter1_scenedata);
        _registry.emplace<components::SceneNode>(emitter1, &emitter1_scenenode);
    }

    void ParticleSystem::load_content()
    {
    }

    void ParticleSystem::update(const engine::GameTime &update_time)
    {
        const auto emitter_view = _registry.view<tilegame::components::ParticleEmitter, tilegame::components::Transform, tilegame::components::Shape>();

        for (const auto &&[entity, emitter, transform, shape] : emitter_view.each())
        {
            // TODO Emitter need to have a clock to track the rate correctly!
            int num_new_particles = static_cast<int>(emitter.rate * update_time.elapsed_time) + 1;
            for (int i = 0; i < num_new_particles; ++i)
            {
                float lifetime = get_random(emitter.lifetime.x, emitter.lifetime.y);
                float scale = get_random(emitter.scale.x, emitter.scale.y);
                float angle = get_random(-emitter.spread_angle, emitter.spread_angle);
                glm::vec2 direction = glm::rotate(emitter.spread_direction, angle);
                float speed = get_random(emitter.speed.x, emitter.speed.y);

                const auto result = create_particle(lifetime, scale, emitter.color, direction * speed);
            }
        }
    }

    entt::entity ParticleSystem::create_particle(float lifetime, float scale, engine::Color color, glm::vec2 velocity)
    {
        engine::Log::d(lifetime, scale, velocity.x);
        return entt::entity();
    }
} // namespace tilegame::systems
