#include "weather.hpp"

#include <algorithm>
#include <memory>
#include <vector>

#include "components/particleemitter.hpp"
#include "components/particlepool.hpp"
#include "components/shape.hpp"
#include "components/transform.hpp"
#include "components/renderable2d.hpp"
#include "components/depth.hpp"
#include "components/camera.hpp"
#include "components/pin.hpp"

namespace tilegame::systems
{
    Weather::Weather(engine::Scene &scene, entt::registry &registry)
        : System(scene, registry),
          _start_tint(engine::Color::WHITE),
          _target_tint(engine::Color::WHITE),
          _fade_elapsed(0.0f),
          _fade_duration(0.0f)
    {
    }

    void Weather::load_content()
    {
        // Shared with systems::Daytime, which is the one that loads it (see WorldScene::load_content(),
        // where Daytime::load_content() runs first).
        _daytime_shader = &_scene.game().resource_manager().get<engine::Shader>("daytime_shader");
        _daytime_shader->use();
        _daytime_shader->set("weather_tint", static_cast<glm::vec4>(engine::Color::WHITE));

        // On a dedicated, never Inactive entity, following the same convention as
        // systems::Lightning/Camera's control entities.
        const auto weather_entity = _registry.create();
        _registry.emplace<engine::EventListener<components::SetWeatherPrecipitationEvent>>(
            weather_entity,
            [this](const std::string &, const components::SetWeatherPrecipitationEvent &event, entt::entity, entt::entity)
            {
                stop_precipitation();
                create_precipitation_entity(event.emitter, event.spawn_area);
            },
            entt::null);
        _registry.emplace<engine::EventListener<components::ClearWeatherPrecipitationEvent>>(
            weather_entity,
            [this](const std::string &, const components::ClearWeatherPrecipitationEvent &, entt::entity, entt::entity)
            { stop_precipitation(); },
            entt::null);

        _registry.emplace<engine::EventListener<components::SetWeatherTintEvent>>(
            weather_entity,
            [this](const std::string &, const components::SetWeatherTintEvent &event, entt::entity, entt::entity)
            {
                // Read back whatever is currently on screen (rather than the possibly-unfinished
                // previous fade's target) so re-triggering weather mid-fade doesn't jump.
                float lerp_amount = _fade_duration > 0.0f ? std::min(_fade_elapsed / _fade_duration, 1.0f) : 1.0f;
                _start_tint = engine::Color::lerp(_start_tint, _target_tint, lerp_amount);
                _target_tint = event.target_tint;
                _fade_elapsed = 0.0f;
                _fade_duration = event.fade_duration;
            },
            entt::null);
    }

    entt::entity Weather::create_precipitation_entity(const components::ParticleEmitter &emitter, const engine::Rectangle &spawn_area)
    {
        const auto entity = _registry.create();
        _registry.emplace<components::Precipitation>(entity);
        _registry.emplace<components::Transform>(entity, glm::vec2(0.0f, 0.0f));
        _registry.emplace<components::Shape>(entity, engine::ShapeVariant(spawn_area));
        _registry.emplace<components::ParticleEmitter>(entity, emitter);
        _registry.emplace<components::ParticlePool>(entity);
        _registry.emplace<components::Renderable2D>(entity);
        // Arbitrary placeholder for now - close to the largest Z in SpriteBatch's valid [-1, 1]
        // range (see engine::graphics::SpriteBatch::create()), so weather always wins the
        // (GL_GREATER - larger wins) depth test against tiles/characters and draws in front of
        // them regardless of position - see systems::Render's transparent pass.
        _registry.emplace<components::Depth>(entity, 0.99f);

        // Pinned to the camera (itself Pin'd to player 1, see systems::Camera::load_content())
        // so its spawn area travels with the player's view instead of this system having to
        // recompute it from the camera every frame - the latter spawned particles uniformly
        // across the whole visible screen each frame, which piles up more particles towards the
        // bottom as they fall (each row is fed by everything spawned above it, not just its own
        // row). Pinned to the camera rather than directly to the player so precipitation still
        // tracks the view if the camera is ever re-pinned to something other than player 1. The
        // spawn area itself is Lua's call - see SetWeatherPrecipitationEvent - and is relative to
        // wherever this Transform ends up. Already-spawned particles don't follow the pin (their
        // positions are world-space), so a stopped effect's last drops just fall where they are.
        const auto camera_entity = _registry.ctx().get<entt::entity>(components::CAMERA_ENTITY_ID);
        _registry.emplace<components::Pin>(entity, camera_entity);

        return entity;
    }

    void Weather::stop_precipitation()
    {
        const auto emitting = _registry.view<const components::Precipitation, const components::ParticleEmitter>();
        // Collected first: removing ParticleEmitter while iterating a view over it would
        // invalidate the iteration.
        const std::vector<entt::entity> entities(emitting.begin(), emitting.end());
        _registry.remove<components::ParticleEmitter>(entities.begin(), entities.end());
    }

    void Weather::destroy_finished_precipitation()
    {
        std::vector<entt::entity> finished;
        const auto stopped = _registry.view<const components::Precipitation, const components::ParticlePool>(entt::exclude<components::ParticleEmitter>);
        for (const auto entity : stopped)
        {
            if (stopped.get<const components::ParticlePool>(entity).first_dead_particle == 0)
            {
                finished.push_back(entity);
            }
        }

        for (const auto entity : finished)
        {
            const auto &pool = _registry.get<const components::ParticlePool>(entity);
            _registry.destroy(pool.container.begin(), pool.container.end());
            _registry.destroy(entity);
        }
    }

    void Weather::update(const engine::GameTime &update_time)
    {
        destroy_finished_precipitation();

        _fade_elapsed = std::min(_fade_elapsed + update_time.elapsed_time, _fade_duration);
        float lerp_amount = _fade_duration > 0.0f ? _fade_elapsed / _fade_duration : 1.0f;

        _daytime_shader->use();
        _daytime_shader->set("weather_tint", static_cast<glm::vec4>(engine::Color::lerp(_start_tint, _target_tint, lerp_amount)));
    }
} // namespace tilegame::systems
