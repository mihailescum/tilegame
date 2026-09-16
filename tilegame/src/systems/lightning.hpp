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
     * Owns a single lightning entity, created once in load_content(), carrying a
     * components::Timer (schedules the next strike; decremented by the shared systems::Timer,
     * so this system never rolls its own elapsed-time bookkeeping for that part), a
     * components::Lightning (remembers the configured [min_interval, max_interval) range, and
     * flash_duration, so each strike can reschedule itself and know how long its flash should
     * take to decay), and its own EventListener<TimerEvent> (source-filtered to itself) that,
     * when the Timer rings, calls strike() - which raises a components::LightningEvent via the
     * inherited System::raise() for any Lua/native listener (via Script::add_event_listener) and
     * reschedules the Timer to a fresh random interval. "No lightning scheduled" is represented
     * by the lightning entity carrying an Inactive tag - so a *separate* control entity (also
     * created in load_content(), never tagged Inactive, since raise_event()'s listener view
     * excludes Inactive entities and this is exactly the event meant to remove that tag) carries
     * EventListener<SetLightningEvent>/EventListener<ClearLightningEvent>, which react the
     * moment Script::set_lightning()/clear_lightning() raises them (see
     * components/lightning.hpp) by mutating the lightning entity from the outside.
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

        // Raises a LightningEvent, starts the visual flash, and reschedules `lightning_entity`'s
        // Timer to a fresh random interval within its configured range.
        void strike(entt::entity lightning_entity);

    public:
        Lightning(tilegame::Scene &scene, entt::registry &registry);

        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
