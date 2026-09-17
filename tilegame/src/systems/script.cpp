#include "script.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

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
#include "components/facing.hpp"
#include "components/interactable.hpp"
#include "components/currentmap.hpp"
#include "components/map.hpp"
#include "components/shape.hpp"
#include "systems/world.hpp"
#include "components/messagebox.hpp"
#include "components/player.hpp"
#include "components/ordering.hpp"
#include "components/renderable2d.hpp"
#include "components/sprite.hpp"
#include "components/animation.hpp"
#include "components/collider.hpp"
#include "components/tilelayer.hpp"
#include "components/spriteorientation.hpp"

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
        run_script("content/scripts/maploader.lua");
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
            throw std::runtime_error("Error loading file");
        }
    }

    json11::Json Script::read_json_file(const std::string &path) const
    {
        std::ifstream file(path);
        if (!file)
        {
            throw std::runtime_error("Error opening JSON file");
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        std::string err;
        json11::Json json = json11::Json::parse(buffer.str(), err);
        if (!err.empty())
        {
            throw std::runtime_error("Error parsing JSON file");
        }

        return json;
    }

    sol::table Script::load_json(const std::string &path)
    {
        return json_to_lua(_lua(), read_json_file(path)).as<sol::table>();
    }

    sol::object Script::json_to_lua(sol::state_view lua, const json11::Json &json)
    {
        switch (json.type())
        {
        case json11::Json::NUMBER:
            return sol::make_object(lua, json.number_value());
        case json11::Json::BOOL:
            return sol::make_object(lua, json.bool_value());
        case json11::Json::STRING:
            return sol::make_object(lua, json.string_value());
        case json11::Json::ARRAY:
        {
            sol::table table = lua.create_table();
            int index = 1;
            for (const auto &item : json.array_items())
            {
                table[index++] = json_to_lua(lua, item);
            }
            return table;
        }
        case json11::Json::OBJECT:
        {
            sol::table table = lua.create_table();
            for (const auto &[key, value] : json.object_items())
            {
                table[key] = json_to_lua(lua, value);
            }
            return table;
        }
        case json11::Json::NUL:
        default:
            return sol::make_object(lua, sol::lua_nil);
        }
    }

    const engine::Texture2D *Script::load_texture(const std::string &path)
    {
        const std::filesystem::path fs_path(path);
        return _scene.game().resource_manager().load_resource<engine::Texture2D>(fs_path.filename().string(), fs_path);
    }

    engine::graphics::Sprite &Script::get_or_create_sprite_class(const std::string &class_name)
    {
        return _sprite_classes.try_emplace(class_name, class_name, nullptr).first->second;
    }

    void Script::parse_sprite_animations(const std::string &path)
    {
        if (!_parsed_animation_paths.insert(path).second)
        {
            return;
        }

        const json11::Json json = read_json_file(path);
        const int columns = json["columns"].int_value();
        const glm::ivec2 tile_dimensions(json["tilewidth"].int_value(), json["tileheight"].int_value());

        for (const auto &tile_json : json["tiles"].array_items())
        {
            if (!tile_json["animation"].array_items().empty())
            {
                const std::string class_name = tile_json["type"].string_value();
                get_or_create_sprite_class(class_name).parse(tile_json, columns, tile_dimensions);
            }
        }
    }

    void Script::make_orientable_if_directional(entt::entity entity, const engine::graphics::Sprite &sprite, const std::string &initial_state_name)
    {
        components::SpriteOrientation::make_orientable_if_directional(_registry, entity, sprite, initial_state_name);
    }

    void Script::emplace_collider(entt::entity entity, const sol::table &shape_descriptor)
    {
        _registry.emplace<components::Collider>(entity, components::Collider::make_shape(shape_descriptor));
    }

    void Script::emplace_tilelayer(entt::entity entity, const glm::vec2 &dimensions, const glm::vec2 &tile_dimensions, const sol::table &cells)
    {
        _registry.emplace<components::TileLayer>(entity, components::TileLayer::build(glm::ivec2(dimensions), glm::ivec2(tile_dimensions), cells));
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
        // Opaque handle returned by `_load_texture`, passed on to `_Sprite`/`_TileLayer`
        // constructors - not directly constructible or readable from Lua.
        _lua().new_usertype<engine::Texture2D>(
            "_Texture", sol::no_constructor);
        // Opaque handle returned by `_get_or_create_sprite_class` (a tileset's per-class
        // animation data, parsed by `_parse_sprite_animations`), passed on to
        // `_Animation`/`_make_orientable_if_directional`.
        _lua().new_usertype<engine::graphics::Sprite>(
            "_SpriteClass", sol::no_constructor);

        components::Direction::register_component(_lua());
        components::Facing::register_component(_lua());
        components::Inactive::register_component(_lua());
        components::Interactable::register_component(_lua());
        components::InteractEvent::register_component(_lua());
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
        components::Ordering::register_component(_lua());
        components::Renderable2D::register_component(_lua());
        components::Sprite::register_component(_lua());
        components::Animation::register_component(_lua());
        components::Map::register_component(_lua());
        components::Shape::register_component(_lua());

        _lua().set_function("_load_json", &Script::load_json, this);
        _lua().set_function("_load_texture", &Script::load_texture, this);
        _lua().set_function("_get_or_create_sprite_class", &Script::get_or_create_sprite_class, this);
        _lua().set_function("_parse_sprite_animations", &Script::parse_sprite_animations, this);
        _lua().set_function("_make_orientable_if_directional", &Script::make_orientable_if_directional, this);
        _lua().set_function("_emplace_collider", &Script::emplace_collider, this);
        _lua().set_function("_emplace_tilelayer", &Script::emplace_tilelayer, this);

        _lua().set_function("_add_event_listener",
                            sol::overload(
                                [this](const sol::table &event, sol::function callback)
                                { return Script::add_event_listener(event, callback, entt::null); },
                                [this](const sol::table &event, sol::function callback, entt::entity source)
                                { return Script::add_event_listener(event, callback, source); }));
        _lua().set_function("_remove_event_listener", &Script::remove_event_listener, this);
        _lua().set_function("_to_global", sol::resolve<glm::vec2(entt::entity, const glm::vec2 &) const>(&Script::to_global), this);
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
        register_event_type<components::InteractEvent, components::EventListener<components::InteractEvent>>();
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

    glm::vec2 Script::to_global(entt::entity map, const glm::vec2 &relative_position) const
    {
        return systems::World::to_global(_registry, map, relative_position);
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
                throw std::runtime_error("Error loading file");
            }

            _registry.erase<components::ScriptLoader>(entity);
        }
    }
} // namespace tilegame::systems
