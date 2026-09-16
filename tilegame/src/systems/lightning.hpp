#pragma once

#include "engine.hpp"
#include "entt/entt.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    /**
     * @brief Schedules recurring lightning strikes at random intervals and drives the screen
     * flash.
     *
     * Owns a single lightning entity, created once in load_content() and tracked via a private
     * registry context id rather than a system member (see lightning.cpp), carrying a
     * components::Timer (schedules the next strike; decremented by the shared systems::Timer,
     * so this system never rolls its own elapsed-time bookkeeping for that part), a
     * components::Lightning (remembers the configured [min_interval, max_interval) range, and
     * flash_duration, so each strike can reschedule itself and know how long its flash should
     * take to decay), and its own components::EventListener<TimerEvent> (source-filtered to
     * itself) that, when the Timer rings, raises a components::LightningEvent for any
     * Lua/native listener (via Script::add_event_listener) and reschedules the Timer to a
     * fresh random interval - the same native-event mechanism systems::Camera's screen shake
     * uses to react to its own Timer. "No lightning scheduled" is represented by that entity
     * carrying an Inactive tag, toggled by components::SetLightningEvent/ClearLightningEvent
     * (raised by Script::set_lightning()/clear_lightning(), on their own throwaway entities).
     *
     * Separately, every frame it decays a private flash intensity towards 0 over the struck
     * flash_duration (set to 1/that duration by the same Timer reaction) and uploads the
     * intensity to the daytime post-processing shader's `flash_intensity` uniform (see
     * content/shaders/daytime.frag), which briefly brightens the whole scene towards white -
     * independent of the strike schedule, so an in-progress flash still finishes playing out
     * even if the schedule is cleared mid-flash.
     */
    class Lightning : public System
    {
    private:
        engine::Shader *_daytime_shader;
        float _flash_intensity;
        // How long the current/most recent flash takes to decay from full intensity back to 0;
        // set from components::Lightning::flash_duration by strike() each time it fires.
        float _flash_duration;

        void apply_pending_commands();
        // Raises a LightningEvent (on its own throwaway entity, same convention as
        // systems::MessageBox), starts the visual flash, and reschedules `lightning_entity`'s
        // Timer to a fresh random interval within its configured range.
        void strike(entt::entity lightning_entity);

    public:
        Lightning(tilegame::Scene &scene, entt::registry &registry);

        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
