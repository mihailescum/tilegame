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
#include "components/player.hpp"
#include "components/pin.hpp"

namespace tilegame::systems
{
    namespace
    {
        using namespace entt::literals;
        // Registry context id (registry.ctx()) under which the precipitation entity's handle is
        // stored, for the entire program's life once load_content() has run - not a dedicated
        // component type, and not a system member either. Private to this file; nothing outside
        // systems::Weather needs to know precipitation is tracked this way.
        constexpr auto PRECIPITATION_ENTITY_ID = "weather_precipitation_entity"_hs;
    }

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
        _registry.ctx().emplace_as<entt::entity>(PRECIPITATION_ENTITY_ID, precipitation_entity);
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

        // Pinned to player 1 (same lookup as systems::Camera::load_content()) so its spawn area
        // travels with the player instead of this system having to recompute it from the camera
        // every frame - the latter spawned particles uniformly across the whole visible screen
        // each frame, which piles up more particles towards the bottom as they fall (each row is
        // fed by everything spawned above it, not just its own row). The spawn area itself is
        // Lua's call - see SetWeatherPrecipitationEvent - and is relative to wherever this
        // Transform ends up.
        auto players = _registry.view<const components::Player>(entt::exclude<components::Inactive>);
        for (auto &&[player_entity, player] : players.each())
        {
            if (player.id == 1)
            {
                _registry.emplace<components::Pin>(entity, player_entity);
                break;
            }
        }

        return entity;
    }

    void Weather::apply_pending_commands()
    {
        for (auto &&[entity, event] : _registry.view<components::SetWeatherTintEvent>().each())
        {
            // Read back whatever is currently on screen (rather than the possibly-unfinished
            // previous fade's target) so re-triggering weather mid-fade doesn't jump.
            float lerp_amount = _fade_duration > 0.0f ? std::min(_fade_elapsed / _fade_duration, 1.0f) : 1.0f;
            _start_tint = engine::Color::lerp(_start_tint, _target_tint, lerp_amount);
            _target_tint = event.target_tint;
            _fade_elapsed = 0.0f;
            _fade_duration = event.fade_duration;
            _registry.destroy(entity);
        }

        for (auto &&[entity, event] : _registry.view<components::SetWeatherPrecipitationEvent>().each())
        {
            const auto precipitation_entity = _registry.ctx().get<entt::entity>(PRECIPITATION_ENTITY_ID);
            _registry.replace<components::ParticleEmitter>(precipitation_entity, event.emitter);
            _registry.replace<components::Shape>(precipitation_entity, std::make_unique<engine::Rectangle>(event.spawn_area));
            _registry.remove<components::Inactive>(precipitation_entity);
            _registry.destroy(entity);
        }

        for (const auto entity : _registry.view<components::ClearWeatherPrecipitationEvent>())
        {
            const auto precipitation_entity = _registry.ctx().get<entt::entity>(PRECIPITATION_ENTITY_ID);
            _registry.emplace_or_replace<components::Inactive>(precipitation_entity);
            _registry.destroy(entity);
        }
    }

    void Weather::update(const engine::GameTime &update_time)
    {
        apply_pending_commands();

        _fade_elapsed = std::min(_fade_elapsed + update_time.elapsed_time, _fade_duration);
        float lerp_amount = _fade_duration > 0.0f ? _fade_elapsed / _fade_duration : 1.0f;

        _daytime_shader->use();
        _daytime_shader->set("weather_tint", static_cast<glm::vec4>(engine::Color::lerp(_start_tint, _target_tint, lerp_amount)));
    }
} // namespace tilegame::systems
