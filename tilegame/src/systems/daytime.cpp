#include "daytime.hpp"

#include <algorithm>

namespace tilegame::systems
{
    Daytime::Daytime(engine::Scene &scene, entt::registry &registry)
        : System(scene, registry), _now(0), _day_duration(0), _speedup(1.0)
    {
    }

    void Daytime::load_content()
    {

        _registry.ctx().insert_or_assign<float>(components::DAYTIME_TIME_ID, 0.0f);

        _daytime_shader = _scene.game().resource_manager().load_resource<engine::Shader>(
            "daytime_shader",
            "content/shaders/daytime",
            "content/shaders/quad.vert", "", "content/shaders/daytime.frag");
        _daytime_shader->use();
        _daytime_shader->set("scene", 0);
        _daytime_shader->set("daytime_tint", 1);

        auto blend_shader = _scene.game().resource_manager().load_resource<engine::Shader>(
            "blend_shader",
            "content/shaders/additive_blend",
            "content/shaders/quad.vert", "", "content/shaders/additive_blend.frag");
        blend_shader->use();
        blend_shader->set("scene", 0);
        blend_shader->set("bloomBlur", 1);
        blend_shader->set("exposure", 1.0f);

        const auto entity = _registry.create();
        _registry.emplace<engine::EventListener<components::SetDaytimeTimeEvent>>(
            entity,
            [this](const std::string &, const components::SetDaytimeTimeEvent &event, entt::entity, entt::entity)
            { _now = event.seconds_since_midnight; },
            entt::null);
        _registry.emplace<engine::EventListener<components::SetDaytimeSpeedupEvent>>(
            entity,
            [this](const std::string &, const components::SetDaytimeSpeedupEvent &event, entt::entity, entt::entity)
            { _speedup = event.speedup; },
            entt::null);
        _registry.emplace<engine::EventListener<components::SetDaytimeDayDurationEvent>>(
            entity,
            [this](const std::string &, const components::SetDaytimeDayDurationEvent &event, entt::entity, entt::entity)
            { _day_duration = event.seconds; },
            entt::null);
    }

    void Daytime::update(const engine::GameTime &update_time)
    {
        _now += static_cast<int>(_speedup * update_time.elapsed_time);
        if (_day_duration > 0)
        {
            if (_now >= _day_duration)
            {
                _now -= _day_duration;
            }
        }
        else
        {
            _now = 0;
        }

        const float time = _day_duration > 0 ? static_cast<float>(_now) / _day_duration : 0.0f;
        _daytime_shader->use();
        _daytime_shader->set("time", time);

        // Also consumed by systems::Render's luminosity shader, which samples the same daytime
        // texture to derive how dark it is right now - see components::DAYTIME_TIME_ID.
        _registry.ctx().insert_or_assign<float>(components::DAYTIME_TIME_ID, float(time));
    }
} // namespace tilegame::systems
