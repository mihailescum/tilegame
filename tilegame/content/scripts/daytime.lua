-- Global day/night cycle configuration.
--
-- Run once at startup by systems::Script (see Script::initialize()). Defines every
-- keyframe ("mark") of the cycle: the in-game time (seconds after midnight) at which
-- it starts, and the screen tint color the scene should have at that moment.
-- systems::Daytime interpolates linearly between consecutive marks each frame, and
-- wraps from the last mark back to the first across midnight/day_duration.
--
-- Everything here can be changed at runtime too, from any other script, by calling
-- the same _set_daytime_* functions again (e.g. to fade into a storm or a cutscene).

local HOUR = 3600

_set_daytime_day_duration(24 * HOUR)

_set_daytime_marks({
    { start = 0,         color = _Color(0.1, 0.1, 0.1, 1.0) },   -- midnight
    { start = 5 * HOUR,  color = _Color(0.5, 0.5, 0.5, 1.0) },   -- pre-dawn
    { start = 7 * HOUR,  color = _Color(1.0, 0.902, 0.8, 1.0) }, -- sunrise
    { start = 9 * HOUR,  color = _Color(1.0, 1.0, 1.0, 1.0) },   -- morning
    { start = 18 * HOUR, color = _Color(1.0, 1.0, 1.0, 1.0) },   -- afternoon
    { start = 20 * HOUR, color = _Color(1.0, 0.518, 0.0, 1.0) }, -- sunset
    { start = 22 * HOUR, color = _Color(0.6, 0.6, 0.6, 1.0) },   -- night
})

_set_daytime_time(12 * HOUR)  -- start at noon
_set_daytime_speedup(HOUR)    -- 1 real second = 1 in-game hour
