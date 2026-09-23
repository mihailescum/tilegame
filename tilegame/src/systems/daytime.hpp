#pragma once

#include <vector>

#include "engine.hpp"

#include "components/daytime.hpp"

namespace tilegame::systems
{
    /**
     * @brief Drives the day/night cycle and its screen tint.
     *
     * Owns the cycle's state privately (keyframes, clock, speedup, day length), mutated only by
     * four EventListener<T>s registered on a dedicated entity in load_content() - one per
     * components::SetDaytime*Event - which react the instant Lua's matching `_set_daytime_*`
     * binding raises them via the inherited System::raise(), which is the only way this state
     * ever changes. Every frame, advances the clock, finds the surrounding keyframes and
     * interpolates between them, uploading the result to the daytime post-processing shader,
     * and also derives a perceptual brightness from it, stored as components::NIGHT_AMOUNT_ID in
     * the registry context for systems::Render to read (see spritebatch_luminosity.frag) so
     * light sources only bloom once it's actually dark. This system has no knowledge of
     * systems::Script or Lua at all; there is currently no way to read the cycle's state back
     * from Lua (only to set it).
     */
    class Daytime : public engine::System
    {
    private:
        engine::Shader *_daytime_shader;

        std::vector<components::TimeOfDayMark> _times_of_day;
        int _now;          // Seconds from midnight
        int _day_duration; // Length of a full day/night cycle, in seconds
        double _speedup;   // How many ingame seconds pass in one real world second

    public:
        Daytime(engine::Scene &scene, entt::registry &registry);

        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
