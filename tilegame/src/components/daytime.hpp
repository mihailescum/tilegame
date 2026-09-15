#pragma once

#include <vector>

#include "engine.hpp"

namespace tilegame::components
{
    // A keyframe of the day/night cycle: the tint color the scene should have starting at
    // `start` seconds after midnight, interpolated towards the next mark's color over time.
    struct TimeOfDayMark
    {
        int start;
        engine::Color tint_color;

        TimeOfDayMark(int start, const engine::Color &tint_color) : start(start), tint_color(tint_color) {}
    };

    // ---- Commands: emplaced on a fresh entity by the _set_daytime_* Lua bindings (see
    // Script::set_daytime_marks() etc.), consumed and destroyed by systems::Daytime the next
    // time it updates. systems::Daytime is their only subscriber, so they skip the generic
    // EventListener<T>/raise_events() broadcast machinery (that's for notifying arbitrary Lua
    // listeners of native events) and are just picked up directly.

    struct SetDaytimeMarksEvent
    {
        std::vector<TimeOfDayMark> marks;
    };

    struct SetDaytimeTimeEvent
    {
        int seconds_since_midnight;
    };

    struct SetDaytimeSpeedupEvent
    {
        double speedup;
    };

    struct SetDaytimeDayDurationEvent
    {
        int seconds;
    };
} // namespace tilegame::components
