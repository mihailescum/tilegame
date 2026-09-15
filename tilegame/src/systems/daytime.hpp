#pragma once

#include <string>
#include <vector>

#include "engine.hpp"

#include "system.hpp"

namespace tilegame::systems
{
    // A keyframe of the day/night cycle: the tint color the scene should have starting at
    // `start` seconds after midnight, interpolated towards the next mark's color over time.
    struct TimeOfDayMark
    {
        int start;
        engine::Color tint_color;

        TimeOfDayMark(int start, const engine::Color &tint_color) : start(start), tint_color(tint_color) {}
    };

    /**
     * @brief Drives the day/night cycle and its screen tint.
     *
     * Owns no entities/components; instead advances an in-game clock each
     * frame and, based on a fixed sequence of TimeOfDayMark keyframes (e.g.
     * midnight, dawn, morning, dusk), interpolates the current tint color and
     * uploads it to the daytime post-processing shader.
     */
    class Daytime : public System
    {
    private:
        static const int DAY_DURATION = 86400;

        engine::Shader *_daytime_shader;

        std::vector<TimeOfDayMark> _times_of_day;
        std::vector<TimeOfDayMark>::iterator _now_mark;
        int _now;        // Seconds from midnight
        double _speedup; // How many ingame seconds pass in one real world second

    public:
        Daytime(tilegame::Scene &scene, entt::registry &registry);

        void initialize();
        void load_content();
        void update(const engine::GameTime &update_time);
    };
} // namespace tilegame