#pragma once

#include <string>
#include <vector>

#include "entt/entt.hpp"

#include "engine.hpp"

namespace tilegame::components
{
    // Registry context id (registry.ctx()), under which the current darkness amount (0 in full
    // daylight, 1 at full night) is stored as a plain float - updated every frame by
    // systems::Daytime, read by systems::Render to gate how strongly light sources bloom (see
    // content/shaders/spritebatch_luminosity.frag). Shared between the two systems, hence
    // living here rather than privately in either .cpp - the same reasoning as
    // components::CAMERA_ENTITY_ID.
    inline constexpr entt::id_type NIGHT_AMOUNT_ID = entt::hashed_string("daytime_night_amount").value();

    // A keyframe of the day/night cycle: the tint color the scene should have starting at
    // `start` seconds after midnight, interpolated towards the next mark's color over time.
    struct TimeOfDayMark
    {
        int start;
        engine::Color tint_color;

        TimeOfDayMark(int start, const engine::Color &tint_color) : start(start), tint_color(tint_color) {}
    };

    // ---- Raised via System::raise() by the _set_daytime_* Lua bindings (see
    // Script::set_daytime_marks() etc.), immediately delivered to systems::Daytime's matching
    // EventListener<T> (registered in Daytime::load_content()). Not exposed to Lua as
    // usertypes - nothing outside systems::Daytime subscribes to them today - but each carries
    // EVENT_TYPE like any other event since System::raise_event() needs it regardless.

    struct SetDaytimeMarksEvent
    {
        inline static const std::string EVENT_TYPE = "SET_DAYTIME_MARKS_EVENT";

        std::vector<TimeOfDayMark> marks;
    };

    struct SetDaytimeTimeEvent
    {
        inline static const std::string EVENT_TYPE = "SET_DAYTIME_TIME_EVENT";

        int seconds_since_midnight;
    };

    struct SetDaytimeSpeedupEvent
    {
        inline static const std::string EVENT_TYPE = "SET_DAYTIME_SPEEDUP_EVENT";

        double speedup;
    };

    struct SetDaytimeDayDurationEvent
    {
        inline static const std::string EVENT_TYPE = "SET_DAYTIME_DAY_DURATION_EVENT";

        int seconds;
    };
} // namespace tilegame::components
