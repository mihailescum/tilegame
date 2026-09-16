#include "script.hpp"

#include <algorithm>

#include "entt_sol/bond.hpp"

#include "components/scriptloader.hpp"
#include "components/timer.hpp"
#include "components/luatable.hpp"
#include "components/inactive.hpp"
#include "components/target.hpp"
#include "components/speed.hpp"
#include "components/transform.hpp"
#include "components/pin.hpp"
#include "components/direction.hpp"
#include "components/currentmap.hpp"
#include "components/messagebox.hpp"
#include "components/player.hpp"

#define AUTO_ARG(x) decltype(x), x

namespace tilegame::systems
{
    Script::Script(tilegame::Scene &scene, entt::registry &registry) : System(scene, registry)
    {
    }

    Script::~Script()
    {
        // Clear all components before the lua state is destroyed to avoid invalid references which would lead to a SegFault
        _registry.clear();
    }

    void Script::initialize()
    {
        _lua.open_libraries();
        register_api();
    }

    void Script::load_content()
    {
        // Global configuration scripts, run once at startup - deliberately here rather than in
        // initialize(), see the declaration in script.hpp for why.
        run_script("content/scripts/daytime.lua");
        run_script("content/scripts/weather.lua");
    }

    void Script::run_script(const std::string &path)
    {
        sol::load_result load_result = _lua().load_file(path);
        if (load_result.valid())
        {
            load_result();
        }
        else
        {
            throw "Error loading file";
        }
    }

    void Script::register_api()
    {
        _lua().require("_registry", sol::c_call<AUTO_ARG(&entt_sol::open_registry)>, false);
        _lua()["_registry"] = std::ref(_registry);

        // Define the user types
        _lua().new_usertype<glm::vec2>(
            "vec2",
            sol::call_constructor,
            sol::factories(
                []()
                { return glm::vec2(0.0); },
                [](float x, float y)
                { return glm::vec2(x, y); }),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y);
        _lua().new_usertype<entt::entity>(
            "_entity", sol::no_constructor);
        _lua().new_usertype<engine::Color>(
            "_Color",
            sol::call_constructor,
            sol::factories(
                []()
                { return engine::Color(); },
                [](float r, float g, float b, float a)
                { return engine::Color(r, g, b, a); }),
            "r", sol::property(sol::resolve<float() const>(&engine::Color::r), sol::resolve<void(float)>(&engine::Color::r)),
            "g", sol::property(sol::resolve<float() const>(&engine::Color::g), sol::resolve<void(float)>(&engine::Color::g)),
            "b", sol::property(sol::resolve<float() const>(&engine::Color::b), sol::resolve<void(float)>(&engine::Color::b)),
            "a", sol::property(sol::resolve<float() const>(&engine::Color::a), sol::resolve<void(float)>(&engine::Color::a)));
        _lua().new_usertype<engine::Rectangle>(
            "_Rectangle",
            sol::call_constructor,
            sol::factories(
                []()
                { return engine::Rectangle(); },
                [](const glm::vec2 &position, const glm::vec2 &dimensions)
                { return engine::Rectangle(position, dimensions); }),
            "position", &engine::Rectangle::position,
            "dimensions", &engine::Rectangle::dimensions);

        components::Direction::register_component(_lua());
        components::Inactive::register_component(_lua());
        components::LuaTable::register_component(_lua());
        components::MapEnteredEvent::register_component(_lua());
        components::MapLeftEvent::register_component(_lua());
        components::MessageOpenedEvent::register_component(_lua());
        components::MessageClosedEvent::register_component(_lua());
        components::LightningEvent::register_component(_lua());
        components::ParticleEmitter::register_component(_lua());
        components::Pin::register_component(_lua());
        components::ScriptLoader::register_component(_lua());
        components::Target::register_component(_lua());
        components::TargetReachedEvent::register_component(_lua());
        components::Timer::register_component(_lua());
        components::TimerEvent::register_component(_lua());
        components::Transform::register_component(_lua());
        components::Speed::register_component(_lua());

        _lua().set_function("_add_event_listener",
                            sol::overload(
                                [this](const sol::table &event, sol::function callback)
                                { return Script::add_event_listener(event, callback, entt::null); },
                                [this](const sol::table &event, sol::function callback, entt::entity source)
                                { return Script::add_event_listener(event, callback, source); }));
        _lua().set_function("_remove_event_listener", &Script::remove_event_listener, this);
        _lua().set_function("_to_global", sol::resolve<glm::vec2(const std::string &, const glm::vec2 &) const>(&Script::to_global), this);
        _lua().set_function("_show_message",
                            sol::overload(
                                [this](const std::string &text)
                                { Script::show_message(text, false, {}); },
                                [this](const std::string &text, bool append)
                                { Script::show_message(text, append, {}); },
                                [this](const std::string &text, bool append, const sol::table &options)
                                {
                                    std::vector<std::string> parsed_options;
                                    for (const auto &option : options)
                                    {
                                        parsed_options.push_back(option.second.as<std::string>());
                                    }
                                    Script::show_message(text, append, parsed_options);
                                }));
        _lua().set_function("_set_daytime_marks", &Script::set_daytime_marks, this);
        _lua().set_function("_set_daytime_time", &Script::set_daytime_time, this);
        _lua().set_function("_set_daytime_speedup", &Script::set_daytime_speedup, this);
        _lua().set_function("_set_daytime_day_duration", &Script::set_daytime_day_duration, this);
        _lua().set_function("_set_weather_tint", &Script::set_weather_tint, this);
        _lua().set_function("_set_weather_precipitation",
                            sol::overload([this]()
                                          { Script::set_weather_precipitation(); },
                                          [this](const components::ParticleEmitter &emitter, const engine::Rectangle &spawn_area)
                                          { Script::set_weather_precipitation(emitter, spawn_area); }));
        _lua().set_function("_shake_camera_horizontal", &Script::shake_camera_horizontal, this);
        _lua().set_function("_shake_camera_vertical", &Script::shake_camera_vertical, this);
        _lua().set_function("_set_lightning", &Script::set_lightning, this);
        _lua().set_function("_clear_lightning", &Script::clear_lightning, this);
        _lua().set_function("_stop_player_input", &Script::stop_player_input, this);
        _lua().set_function("_resume_player_input", &Script::resume_player_input, this);
        register_event_type<components::TargetReachedEvent, components::EventListener<components::TargetReachedEvent>>();
        register_event_type<components::TimerEvent, components::EventListener<components::TimerEvent>>();
        register_event_type<components::MapEnteredEvent, components::EventListener<components::MapEnteredEvent>>();
        register_event_type<components::MapLeftEvent, components::EventListener<components::MapLeftEvent>>();
        register_event_type<components::MessageClosedEvent, components::EventListener<components::MessageClosedEvent>>();
        register_event_type<components::MessageOpenedEvent, components::EventListener<components::MessageOpenedEvent>>();
        register_event_type<components::LightningEvent, components::EventListener<components::LightningEvent>>();
    }

    entt::entity Script::add_event_listener(const sol::table &event, sol::function callback, entt::entity source)
    {
        auto event_type = event["EVENT_TYPE"];
        if (event_type.valid() && _event_types.find(event_type) != _event_types.end())
        {
            return _event_types[event_type](callback, source);
        }
        else
        {
            return entt::null;
        }
    }

    void Script::remove_event_listener(entt::entity listener)
    {
        if (_registry.valid(listener))
        {
            _registry.destroy(listener);
        }
    }

    glm::vec2 Script::to_global(const std::string &map_name, const glm::vec2 &relative_position) const
    {
        const auto &world = _scene.game().resource_manager().get<engine::tilemap::World>("world1");
        return world.to_global(map_name, relative_position);
    }

    void Script::show_message(const std::string &text, bool append, const std::vector<std::string> &options)
    {
        raise_event<components::ShowMessageEvent>(entt::null, text, append, options);
    }

    void Script::set_daytime_marks(const sol::table &marks)
    {
        std::vector<components::TimeOfDayMark> parsed;
        for (const auto &entry : marks)
        {
            const sol::table mark = entry.second.as<sol::table>();
            parsed.emplace_back(mark["start"].get<int>(), mark["color"].get<engine::Color>());
        }
        std::sort(parsed.begin(), parsed.end(), [](const components::TimeOfDayMark &a, const components::TimeOfDayMark &b)
                  { return a.start < b.start; });

        raise_event<components::SetDaytimeMarksEvent>(entt::null, std::move(parsed));
    }

    void Script::set_daytime_time(int seconds_since_midnight)
    {
        raise_event<components::SetDaytimeTimeEvent>(entt::null, seconds_since_midnight);
    }

    void Script::set_daytime_speedup(double ingame_seconds_per_real_second)
    {
        raise_event<components::SetDaytimeSpeedupEvent>(entt::null, ingame_seconds_per_real_second);
    }

    void Script::set_daytime_day_duration(int seconds)
    {
        raise_event<components::SetDaytimeDayDurationEvent>(entt::null, seconds);
    }

    void Script::set_weather_tint(const engine::Color &target_tint, float fade_duration)
    {
        raise_event<components::SetWeatherTintEvent>(entt::null, target_tint, fade_duration);
    }

    void Script::set_weather_precipitation(const components::ParticleEmitter &emitter, const engine::Rectangle &spawn_area)
    {
        raise_event<components::SetWeatherPrecipitationEvent>(entt::null, emitter, spawn_area);
    }

    void Script::set_weather_precipitation()
    {
        raise_event<components::ClearWeatherPrecipitationEvent>();
    }

    void Script::shake_camera_horizontal(float displacement_speed, float offset, float duration)
    {
        raise_event<components::ShakeCameraHorizontalEvent>(entt::null, displacement_speed, offset, duration);
    }

    void Script::shake_camera_vertical(float displacement_speed, float offset, float duration)
    {
        raise_event<components::ShakeCameraVerticalEvent>(entt::null, displacement_speed, offset, duration);
    }

    void Script::set_lightning(float min_interval, float max_interval, float flash_duration)
    {
        raise_event<components::SetLightningEvent>(entt::null, min_interval, max_interval, flash_duration);
    }

    void Script::clear_lightning()
    {
        raise_event<components::ClearLightningEvent>();
    }

    void Script::stop_player_input(int player_id)
    {
        raise_event<components::StopPlayerInputEvent>(entt::null, player_id);
    }

    void Script::resume_player_input(int player_id)
    {
        raise_event<components::ResumePlayerInputEvent>(entt::null, player_id);
    }

    void Script::update(const engine::GameTime &update_time)
    {
        // TODO loading scripts should happen somewhere else, not on update

        const auto script_entities = _registry.view<const components::ScriptLoader>(entt::exclude<components::Inactive>);
        for (const auto &&[entity, script_loader] : script_entities.each())
        {
            const auto &script_path = script_loader.path;
            sol::load_result load_result = _lua().load_file(script_path);
            if (load_result.valid())
            {
                sol::table arg = _lua().create_table_with("entity", entity);
                sol::table result = load_result(arg);
                _registry.emplace<components::LuaTable>(entity, result);
            }
            else
            {
                throw "Error loading file";
            }

            _registry.erase<components::ScriptLoader>(entity);
        }
    }
} // namespace tilegame::systems
