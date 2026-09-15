#include "daytime.hpp"

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

        auto blend_shader = _scene.game().resource_manager().load_resource<engine::Shader>(
            "blend_shader",
            "content/shaders/additive_blend",
            "content/shaders/quad.vert", "", "content/shaders/additive_blend.frag");
        blend_shader->use();
        blend_shader->set("scene", 0);
        blend_shader->set("bloomBlur", 1);
        blend_shader->set("exposure", 1.0f);
    }

    void Daytime::apply_pending_commands()
    {
        for (auto &&[entity, event] : _registry.view<components::SetDaytimeMarksEvent>().each())
        {
            _times_of_day = std::move(event.marks);
            _registry.destroy(entity);
        }
        for (auto &&[entity, event] : _registry.view<components::SetDaytimeTimeEvent>().each())
        {
            _now = event.seconds_since_midnight;
            _registry.destroy(entity);
        }
        for (auto &&[entity, event] : _registry.view<components::SetDaytimeSpeedupEvent>().each())
        {
            _speedup = event.speedup;
            _registry.destroy(entity);
        }
        for (auto &&[entity, event] : _registry.view<components::SetDaytimeDayDurationEvent>().each())
        {
            _day_duration = event.seconds;
            _registry.destroy(entity);
        }
    }

    void Daytime::update(const engine::GameTime &update_time)
    {
        apply_pending_commands();

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

            _daytime_shader->use();
            _daytime_shader->set("tint_color", static_cast<glm::vec4>(engine::Color::lerp(now_mark->tint_color, next_mark->tint_color, lerp_amount)));
        }
    }
} // namespace tilegame::systems
