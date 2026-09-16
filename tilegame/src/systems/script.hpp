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
     * subscribe to. Every `_set_X`/`_show_message`/`_stop_player_input`-style binding raises
     * its event via the inherited System::raise(), which delivers it to whatever native
     * EventListener<T> the owning system registered - synchronously, before the binding
     * returns - so nothing is queued or polled for. In load_content(), once every other
     * system has finished creating the entities/listeners those events target, runs the
     * global (non-entity) configuration scripts such as content/scripts/daytime.lua once.
     * Each frame, also operates on entities with a ScriptLoader component: loads and runs
     * their Lua file once (passing the owning entity), attaches the returned table as a
     * LuaTable component, and removes the ScriptLoader so the script isn't re-run.
     */
    class Script : public System
    {
    private:
        tilegame::SecureLuaState _lua;
        // Maps a Lua-visible EVENT_TYPE name to a function that creates the matching native
        // EventListener<EventType> entity and returns it, so add_event_listener can dispatch
        // generically without knowing the concrete event type at the call site.
        std::unordered_map<std::string, std::function<entt::entity(sol::function, entt::entity)>> _event_types;

        void register_api();
        // Loads and immediately runs a Lua file with no entity argument, for global
        // (non-entity) configuration scripts such as content/scripts/daytime.lua.
        void run_script(const std::string &path);

        // Exposed to Lua as `_set_daytime_marks`; raises a SetDaytimeMarksEvent, immediately
        // delivered to systems::Daytime, wholesale-replacing the day/night cycle's keyframes
        // from a table of {start = <seconds>, color = _Color(...)} entries.
        void set_daytime_marks(const sol::table &marks);
        // Exposed to Lua as `_set_daytime_time`; the in-game clock, in seconds since midnight.
        // Raises a SetDaytimeTimeEvent, immediately delivered to systems::Daytime.
        void set_daytime_time(int seconds_since_midnight);
        // Exposed to Lua as `_set_daytime_speedup`; how many in-game seconds pass per
        // real-world second. Raises a SetDaytimeSpeedupEvent, immediately delivered to
        // systems::Daytime.
        void set_daytime_speedup(double ingame_seconds_per_real_second);
        // Exposed to Lua as `_set_daytime_day_duration`; the length of a full day/night cycle,
        // in seconds. Raises a SetDaytimeDayDurationEvent, immediately delivered to
        // systems::Daytime.
        void set_daytime_day_duration(int seconds);
        // Exposed to Lua as `_set_weather_tint`; fades the weather overlay tint (multiplied with
        // the day/night tint in content/shaders/daytime.frag) from whatever it currently is
        // towards `target_tint` over `fade_duration` seconds. Raises a SetWeatherTintEvent,
        // immediately delivered to systems::Weather.
        void set_weather_tint(const engine::Color &target_tint, float fade_duration);
        // Exposed to Lua as `_set_weather_precipitation`; `emitter` is a fully-configured
        // _ParticleEmitter (rate, spread, speed, lifetime, scale, color, source_rect) and
        // `spawn_area` a _Rectangle particles spawn within, relative to the precipitation
        // entity's Transform (which tracks player 1 - see
        // systems::Weather::create_precipitation_entity()), both built entirely by the calling
        // script - see content/scripts/weather.lua for the rain/snow presets. Raises a
        // SetWeatherPrecipitationEvent, immediately delivered to systems::Weather, which creates
        // or reconfigures the active precipitation effect.
        void set_weather_precipitation(const components::ParticleEmitter &emitter, const engine::Rectangle &spawn_area);
        // Exposed to Lua as `_set_weather_precipitation` without parameter; stops and removes
        // whatever precipitation effect is currently active, if any. Raises a
        // ClearWeatherPrecipitationEvent, immediately delivered to systems::Weather.
        void set_weather_precipitation();
        // Exposed to Lua as `_shake_camera_horizontal`; (re)starts the camera's horizontal
        // screen shake axis, jittering within [-offset, +offset] world units at up to
        // `displacement_speed` units/second, for `duration` seconds, after which
        // systems::Camera smoothly settles it back to center at the same speed. Raises a
        // ShakeCameraHorizontalEvent, immediately delivered to systems::Camera.
        void shake_camera_horizontal(float displacement_speed, float offset, float duration);
        // Exposed to Lua as `_shake_camera_vertical`; same as shake_camera_horizontal() but for
        // the vertical axis. Raises a ShakeCameraVerticalEvent, immediately delivered to
        // systems::Camera.
        void shake_camera_vertical(float displacement_speed, float offset, float duration);
        // Exposed to Lua as `_set_lightning`; (re)starts recurring lightning strikes, each
        // waiting a fresh random interval within [min_interval, max_interval) seconds after the
        // previous one. Each strike raises a _LightningEvent (subscribable via
        // _add_event_listener) and brightens the scene, decaying back to normal over
        // `flash_duration` seconds. Raises a SetLightningEvent, immediately delivered to
        // systems::Lightning.
        void set_lightning(float min_interval, float max_interval, float flash_duration);
        // Exposed to Lua as `_clear_lightning`; stops the recurring schedule, if any. Raises a
        // ClearLightningEvent, immediately delivered to systems::Lightning.
        void clear_lightning();
        // Exposed to Lua as `_add_event_listener`; looks up the event's EVENT_TYPE in
        // _event_types and, if registered, creates the corresponding listener entity and
        // returns it - pass it to `_remove_event_listener` later to unsubscribe. Returns
        // entt::null if the event type isn't registered.
        entt::entity add_event_listener(const sol::table &event, sol::function callback);
        // Exposed to Lua as `_add_event_listener`; looks up the event's EVENT_TYPE in
        // _event_types and, if registered, creates the corresponding listener entity and
        // returns it. Returns entt::null if the event type isn't registered.
        entt::entity add_event_listener(const sol::table &event, sol::function callback, entt::entity source);
        // Exposed to Lua as `_remove_event_listener`; destroys the listener entity returned by
        // an earlier `_add_event_listener` call, unsubscribing that callback. A no-op if
        // `listener` is already invalid (e.g. entt::null, or removed twice).
        void remove_event_listener(entt::entity listener);
        // Exposed to Lua as `_to_global`; converts coordinates relative to the named
        // map's origin into world-space coordinates via the loaded World resource.
        glm::vec2 to_global(const std::string &map_name, const glm::vec2 &relative_position) const;
        // Exposed to Lua as `_show_message`; raises a components::ShowMessageEvent, immediately
        // delivered to systems::MessageBox, which word-wraps `text` and, per `append`, either
        // adding it to the currently displayed message or replacing it. `options`, if
        // non-empty, replaces whatever options box is currently showing (see
        // components::ShowMessageEvent::options); selecting one currently just closes the
        // message - the return value isn't surfaced anywhere yet.
        void show_message(const std::string &text, bool append, const std::vector<std::string> &options);
        // Exposed to Lua as `_stop_player_input`; raises a components::StopPlayerInputEvent,
        // immediately delivered to systems::Player, suppressing keyboard input for whichever
        // player entity's id matches `player_id`.
        void stop_player_input(int player_id);
        // Exposed to Lua as `_resume_player_input`; raises a components::ResumePlayerInputEvent,
        // immediately delivered to systems::Player, re-enabling keyboard input for whichever
        // player entity's id matches `player_id`.
        void resume_player_input(int player_id);

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
                return listener_entity;
            };
        }

    public:
        Script(tilegame::Scene &scene, entt::registry &registry);
        ~Script();

        void initialize();
        // Runs the global (non-entity) configuration scripts (content/scripts/daytime.lua,
        // weather.lua). Deliberately not in initialize(): those scripts immediately raise
        // events like SetWeatherPrecipitationEvent/ShakeCameraHorizontalEvent/SetLightningEvent,
        // which are only deliverable once their target systems have already created the
        // entities/listeners those events reach - load_content() is called last in
        // WorldScene::load_content(), after every other system's own load_content().
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
