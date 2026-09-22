#include "daytime.hpp"

#include <algorithm>

#include "components/event.hpp"

namespace tilegame::systems
{
    Daytime::Daytime(tilegame::Scene &scene, entt::registry &registry)
        : System(scene, registry), _now(0), _day_duration(0), _speedup(1.0)
    {
    }

    void Daytime::load_content()
    {
        _daytime_shader = _scene.game().resource_manager().load_resource<engine::Shader>(
            "daytime_shader",
            "content/shaders/daytime",
            "content/shaders/quad.vert", "", "content/shaders/daytime.frag");
        _daytime_shader->use();
        _daytime_shader->set("scene", 0);
        _daytime_shader->set("tint_color", static_cast<glm::vec4>(engine::Color::WHITE));
        _registry.ctx().insert_or_assign<float>(components::NIGHT_AMOUNT_ID, 0.0f);

        auto blend_shader = _scene.game().resource_manager().load_resource<engine::Shader>(
            "blend_shader",
            "content/shaders/additive_blend",
            "content/shaders/quad.vert", "", "content/shaders/additive_blend.frag");
        blend_shader->use();
        blend_shader->set("scene", 0);
        blend_shader->set("bloomBlur", 1);
        blend_shader->set("exposure", 1.0f);

        const auto entity = _registry.create();
        _registry.emplace<components::EventListener<components::SetDaytimeMarksEvent>>(
            entity,
            [this](const std::string &, const components::SetDaytimeMarksEvent &event, entt::entity, entt::entity)
            { _times_of_day = event.marks; },
            entt::null);
        _registry.emplace<components::EventListener<components::SetDaytimeTimeEvent>>(
            entity,
            [this](const std::string &, const components::SetDaytimeTimeEvent &event, entt::entity, entt::entity)
            { _now = event.seconds_since_midnight; },
            entt::null);
        _registry.emplace<components::EventListener<components::SetDaytimeSpeedupEvent>>(
            entity,
            [this](const std::string &, const components::SetDaytimeSpeedupEvent &event, entt::entity, entt::entity)
            { _speedup = event.speedup; },
            entt::null);
        _registry.emplace<components::EventListener<components::SetDaytimeDayDurationEvent>>(
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
            _now %= _day_duration;
            if (_now < 0)
            {
                _now += _day_duration;
            }
        }
        else
        {
            _now = 0;
        }

        if (!_times_of_day.empty())
        {
            // _times_of_day is kept sorted ascending by `start` (by Script::set_daytime_marks);
            // find the last mark whose start is at or before `_now` - the keyframe we are
            // currently past.
            auto now_mark = _times_of_day.begin();
            for (auto it = _times_of_day.begin(); it != _times_of_day.end(); ++it)
            {
                if (it->start > _now)
                {
                    break;
                }
                now_mark = it;
            }

            auto next_mark = now_mark + 1;
            // The range after the last mark wraps around to the first mark at _day_duration.
            int next_start = (next_mark != _times_of_day.end()) ? next_mark->start : _day_duration;
            if (next_mark == _times_of_day.end())
            {
                next_mark = _times_of_day.begin();
            }

            float lerp_amount = next_start > now_mark->start
                                    ? static_cast<float>(_now - now_mark->start) / (next_start - now_mark->start)
                                    : 0.0f;

            const engine::Color tint = engine::Color::lerp(now_mark->tint_color, next_mark->tint_color, lerp_amount);
            _daytime_shader->use();
            _daytime_shader->set("tint_color", static_cast<glm::vec4>(tint));

            // Perceptual brightness of the current tint, used as an inverse proxy for "how dark
            // is it right now" - see components::NIGHT_AMOUNT_ID.
            const float luminance = 0.2126f * tint.r() + 0.7152f * tint.g() + 0.0722f * tint.b();
            float night_amount = std::clamp(1.0f - luminance, 0.0f, 1.0f);
            _registry.ctx().insert_or_assign<float>(components::NIGHT_AMOUNT_ID, std::move(night_amount));
        }
    }
} // namespace tilegame::systems
