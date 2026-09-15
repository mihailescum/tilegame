#pragma once

#include <vector>

#include "engine.hpp"

#include "system.hpp"
#include "components/daytime.hpp"

namespace tilegame::systems
{
    /**
     * @brief Drives the day/night cycle and its screen tint.
     *
     * Owns the cycle's state privately (keyframes, clock, speedup, day length). Every frame,
     * it first applies any pending components::SetDaytime*Event commands - raised by Lua via
     * Script, on their own throwaway entities - which is the only way this state ever
     * changes, then advances the clock, finds the surrounding keyframes and interpolates
     * between them, uploading the result to the daytime post-processing shader. This system
     * has no knowledge of systems::Script or Lua at all; there is currently no way to read
     * the cycle's state back from Lua (only to set it).
     */
    class Daytime : public System
    {
    private:
        engine::Shader *_daytime_shader;

        std::vector<components::TimeOfDayMark> _times_of_day;
        int _now;          // Seconds from midnight
        int _day_duration; // Length of a full day/night cycle, in seconds
        double _speedup;   // How many ingame seconds pass in one real world second

        void apply_pending_commands();

    public:
        Daytime(tilegame::Scene &scene, entt::registry &registry);

        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame::systems
