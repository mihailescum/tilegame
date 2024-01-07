#include "daytime.hpp"

namespace tilegame::systems
{
    Daytime::Daytime(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry), _speedup(1.0)
    {
    }

    void Daytime::initialize()
    {
        _times_of_day.push_back(TimeOfDayMark(0, engine::Color(0.3, 0.3, 0.3, 1.0)));           // midnight
        _times_of_day.push_back(TimeOfDayMark(5 * 3600, engine::Color(0.5, 0.5, 0.5, 1.0)));    // dusk
        _times_of_day.push_back(TimeOfDayMark(7 * 3600, engine::Color(1.0, 0.902, 0.8, 1.0)));  // early morning
        _times_of_day.push_back(TimeOfDayMark(9 * 3600, engine::Color(1.0, 1.0, 1.0, 1.0)));    // morning
        _times_of_day.push_back(TimeOfDayMark(18 * 3600, engine::Color(1.0, 1.0, 1.0, 1.0)));   // early evening
        _times_of_day.push_back(TimeOfDayMark(20 * 3600, engine::Color(1.0, 0.518, 0.0, 1.0))); // dawn
        _times_of_day.push_back(TimeOfDayMark(22 * 3600, engine::Color(0.6, 0.6, 0.6, 1.0)));   // night

        _now = 43200;
        _speedup = 3600;
        for (_now_mark = _times_of_day.end() - 1; _now_mark != _times_of_day.begin(); --_now_mark)
        {
            if (_now >= _now_mark->start)
            {
                break;
            }
        }
    }

    void Daytime::load_content()
    {
        _daytime_shader = _scene.game().resource_manager().load_resource<engine::Shader>(
            "daytime_shader",
            "content/shaders/daytime",
            "content/shaders/daytime.vert", "", "content/shaders/daytime.frag");

        engine::graphics::PostProcessingEffect daytime_effect(_scene.game().graphics_device(), *_daytime_shader);
        daytime_effect.add_color_attachments(1);
        daytime_effect.input_textures().push_back(std::ref(daytime_effect.color_attachment_at(0)));
        _scene.game().postprocessor().effects().push_back(std::move(daytime_effect));
    }

    void Daytime::update(const engine::GameTime &update_time)
    {
        _now += _speedup * update_time.elapsed_time;

        std::vector<TimeOfDayMark>::iterator _next_mark = _now_mark + 1;
        if (_next_mark == _times_of_day.end())
        {
            _next_mark = _times_of_day.begin();
        }

        if (_next_mark->start > 0)
        {
            if (_now >= _next_mark->start)
            {
                _now_mark = _next_mark;
                _next_mark = _now_mark + 1;
                if (_next_mark == _times_of_day.end())
                {
                    _next_mark = _times_of_day.begin();
                }
            }
        }
        else
        {
            if (_now >= DAY_DURATION)
            {
                _now -= DAY_DURATION;

                _now_mark = _next_mark;
                _next_mark = _now_mark + 1;
                if (_next_mark == _times_of_day.end())
                {
                    _next_mark = _times_of_day.begin();
                }
            }
        }

        float lerp_amount;
        if (_next_mark->start > 0)
        {
            lerp_amount = static_cast<float>(_now - _now_mark->start) / (_next_mark->start - _now_mark->start);
        }
        else
        {
            lerp_amount = static_cast<float>(_now - _now_mark->start) / (DAY_DURATION - _now_mark->start);
        }

        _daytime_shader->use();
        _daytime_shader->set("tint_color", static_cast<glm::vec4>(engine::Color::lerp(_now_mark->tint_color, _next_mark->tint_color, lerp_amount)));
    }
} // namespace tilegame::systems
