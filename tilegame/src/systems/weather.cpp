#include "weather.hpp"

#include <algorithm>
#include <memory>

#include "components/particleemitter.hpp"
#include "components/particlepool.hpp"
#include "components/shape.hpp"
#include "components/transform.hpp"
#include "components/renderable2d.hpp"
#include "components/ordering.hpp"
#include "components/inactive.hpp"
#include "components/camera.hpp"
#include "components/pin.hpp"
#include "components/event.hpp"

namespace tilegame::systems
{
    Weather::Weather(tilegame::Scene &scene, entt::registry &registry)
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

        const auto precipitation_entity = create_precipitation_entity();
        _registry.emplace<components::Inactive>(precipitation_entity);

        // On a *separate* entity - never tagged Inactive - since `precipitation_entity` starts
        // Inactive and raise_event()'s listener view excludes Inactive entities: attaching
        // these to `precipitation_entity` itself would mean the very event whose job is to
        // remove Inactive could never reach it.
        const auto weather_entity = _registry.create();
        _registry.emplace<components::EventListener<components::SetWeatherPrecipitationEvent>>(
            weather_entity,
            [this, precipitation_entity](const std::string &, const components::SetWeatherPrecipitationEvent &event, entt::entity)
            {
                _registry.replace<components::ParticleEmitter>(precipitation_entity, event.emitter);
                _registry.replace<components::Shape>(precipitation_entity, std::make_unique<engine::Rectangle>(event.spawn_area));
                _registry.remove<components::Inactive>(precipitation_entity);
            },
            entt::null);
        _registry.emplace<components::EventListener<components::ClearWeatherPrecipitationEvent>>(
            weather_entity,
            [this, precipitation_entity](const std::string &, const components::ClearWeatherPrecipitationEvent &, entt::entity)
            { _registry.emplace_or_replace<components::Inactive>(precipitation_entity); },
            entt::null);

        _registry.emplace<components::EventListener<components::SetWeatherTintEvent>>(
            weather_entity,
            [this](const std::string &, const components::SetWeatherTintEvent &event, entt::entity)
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

    entt::entity Weather::create_precipitation_entity()
    {
        const auto entity = _registry.create();
        _registry.emplace<components::Transform>(entity, glm::vec2(0.0f, 0.0f));
        _registry.emplace<components::Shape>(entity, std::make_unique<engine::Rectangle>(glm::vec2(0.0f), glm::vec2(0.0f)));
        _registry.emplace<components::ParticleEmitter>(entity);
        _registry.emplace<components::ParticlePool>(entity);
        _registry.emplace<components::Renderable2D>(entity);
        _registry.emplace<components::Ordering>(entity, 1000.0f);

        // Pinned to the camera (itself Pin'd to player 1, see systems::Camera::load_content())
        // so its spawn area travels with the player's view instead of this system having to
        // recompute it from the camera every frame - the latter spawned particles uniformly
        // across the whole visible screen each frame, which piles up more particles towards the
        // bottom as they fall (each row is fed by everything spawned above it, not just its own
        // row). Pinned to the camera rather than directly to the player so precipitation still
        // tracks the view if the camera is ever re-pinned to something other than player 1. The
        // spawn area itself is Lua's call - see SetWeatherPrecipitationEvent - and is relative to
        // wherever this Transform ends up.
        const auto camera_entity = _registry.ctx().get<entt::entity>(components::CAMERA_ENTITY_ID);
        _registry.emplace<components::Pin>(entity, camera_entity);

        return entity;
    }

    void Weather::update(const engine::GameTime &update_time)
    {
        _fade_elapsed = std::min(_fade_elapsed + update_time.elapsed_time, _fade_duration);
        float lerp_amount = _fade_duration > 0.0f ? _fade_elapsed / _fade_duration : 1.0f;

        _daytime_shader->use();
        _daytime_shader->set("weather_tint", static_cast<glm::vec4>(engine::Color::lerp(_start_tint, _target_tint, lerp_amount)));
    }
} // namespace tilegame::systems
