#pragma once

#include <vector>
#include <unordered_map>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

#include "engine.hpp"

#include "secureluastate.hpp"
#include "system.hpp"
#include "components/event.hpp"
#include "components/daytime.hpp"
#include "components/weather.hpp"
#include "components/particleemitter.hpp"
#include "components/camerashake.hpp"
#include "components/lightning.hpp"

namespace tilegame::systems
{
    /**
     * @brief Bridges the ECS/game to sandboxed Lua scripts.
     *
     * Owns the SecureLuaState, registers ECS components as Lua usertypes and
     * the native event types (TargetReachedEvent, TimerEvent) scripts can
     * subscribe to. At startup, runs global (non-entity) configuration
     * scripts such as content/scripts/daytime.lua once. Each frame, also
     * operates on entities with a ScriptLoader component: loads and runs
     * their Lua file once (passing the owning entity), attaches the
     * returned table as a LuaTable component, and removes the ScriptLoader
     * so the script isn't re-run.
     */
    class Script : public System
    {
    private:
        tilegame::SecureLuaState _lua;
        // Maps a Lua-visible EVENT_TYPE name to a function that creates the matching native
        // EventListener<EventType> entity, so add_event_listener can dispatch generically
        // without knowing the concrete event type at the call site.
        std::unordered_map<std::string, std::function<bool(sol::function, entt::entity)>> _event_types;

        void register_api();
        // Loads and immediately runs a Lua file with no entity argument, for global
        // (non-entity) configuration scripts such as content/scripts/daytime.lua.
        void run_script(const std::string &path);

        // Exposed to Lua as `_set_daytime_marks`; raises a SetDaytimeMarksEvent (on a fresh
        // entity) wholesale-replacing the day/night cycle's keyframes, from a table of
        // {start = <seconds>, color = _Color(...)} entries. Applied by systems::Daytime the
        // next time it updates.
        void set_daytime_marks(const sol::table &marks);
        // Exposed to Lua as `_set_daytime_time`; the in-game clock, in seconds since
        // midnight. Raises a SetDaytimeTimeEvent.
        void set_daytime_time(int seconds_since_midnight);
        // Exposed to Lua as `_set_daytime_speedup`; how many in-game seconds pass per
        // real-world second. Raises a SetDaytimeSpeedupEvent.
        void set_daytime_speedup(double ingame_seconds_per_real_second);
        // Exposed to Lua as `_set_daytime_day_duration`; the length of a full day/night
        // cycle, in seconds. Raises a SetDaytimeDayDurationEvent.
        void set_daytime_day_duration(int seconds);
        // Exposed to Lua as `_set_weather_tint`; fades the weather overlay tint (multiplied with
        // the day/night tint in content/shaders/daytime.frag) from whatever it currently is
        // towards `target_tint` over `fade_duration` seconds. Raises a SetWeatherTintEvent.
        void set_weather_tint(const engine::Color &target_tint, float fade_duration);
        // Exposed to Lua as `_set_weather_precipitation`; `emitter` is a fully-configured
        // _ParticleEmitter (rate, spread, speed, lifetime, scale, color, source_rect) and
        // `spawn_area` a _Rectangle particles spawn within, relative to the precipitation
        // entity's Transform (which tracks player 1 - see
        // systems::Weather::create_precipitation_entity()), both built entirely by the calling
        // script - see content/scripts/weather.lua for the rain/snow presets. Raises a
        // SetWeatherPrecipitationEvent, which systems::Weather uses to create or reconfigure the
        // active precipitation effect.
        void set_weather_precipitation(const components::ParticleEmitter &emitter, const engine::Rectangle &spawn_area);
        // Exposed to Lua as `_set_weather_precipitation` without parameter; stops and removes whatever
        // precipitation effect is currently active, if any. Raises a
        // ClearWeatherPrecipitationEvent.
        void set_weather_precipitation();
        // Exposed to Lua as `_shake_camera_horizontal`; (re)starts the camera's horizontal
        // screen shake axis, jittering within [-offset, +offset] world units at up to
        // `displacement_speed` units/second, for `duration` seconds, after which
        // systems::Camera smoothly settles it back to center at the same speed. Raises a
        // ShakeCameraHorizontalEvent.
        void shake_camera_horizontal(float displacement_speed, float offset, float duration);
        // Exposed to Lua as `_shake_camera_vertical`; same as shake_camera_horizontal() but for
        // the vertical axis. Raises a ShakeCameraVerticalEvent.
        void shake_camera_vertical(float displacement_speed, float offset, float duration);
        // Exposed to Lua as `_set_lightning`; (re)starts recurring lightning strikes, each
        // waiting a fresh random interval within [min_interval, max_interval) seconds after the
        // previous one. Each strike raises a _LightningEvent (subscribable via
        // _add_event_listener) and brightens the scene, decaying back to normal over
        // `flash_duration` seconds. Raises a SetLightningEvent.
        void set_lightning(float min_interval, float max_interval, float flash_duration);
        // Exposed to Lua as `_clear_lightning`; stops the recurring schedule, if any. Raises a
        // ClearLightningEvent.
        void clear_lightning();
        // Exposed to Lua as `_add_event_listener`; looks up the event's EVENT_TYPE in
        // _event_types and, if registered, creates the corresponding listener entity.
        bool add_event_listener(const sol::table &event, sol::function callback);
        // Exposed to Lua as `_add_event_listener`; looks up the event's EVENT_TYPE in
        // _event_types and, if registered, creates the corresponding listener entity.
        bool add_event_listener(const sol::table &event, sol::function callback, entt::entity source);
        // Exposed to Lua as `_to_global`; converts coordinates relative to the named
        // map's origin into world-space coordinates via the loaded World resource.
        glm::vec2 to_global(const std::string &map_name, const glm::vec2 &relative_position) const;
        // Exposed to Lua as `_show_message`; raises a components::ShowMessageEvent (on a fresh
        // entity) that systems::MessageBox picks up next frame, word-wrapping `text` and, per
        // `append`, either adding it to the currently displayed message or replacing it.
        void show_message(const std::string &text, bool append);

        // Registers EventType in _event_types so Lua scripts can listen for it via
        // add_event_listener; instantiated once per native event type in register_api().
        template <class EventType, class EventListener = components::EventListener<EventType>>
        void register_event_type()
        {
            _event_types[EventType::EVENT_TYPE] =
                [&_registry = _registry](sol::function callback, entt::entity source)
            {
                const auto listener_entity = _registry.create();
                _registry.emplace<EventListener>(listener_entity, EventListener(callback, source));
                return true;
            };
        }

    public:
        Script(tilegame::Scene &scene, entt::registry &registry);
        ~Script();

        void initialize();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
