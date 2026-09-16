#pragma once

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
