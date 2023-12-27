#pragma once

#include <string>

#include <entt/entity/registry.hpp>

namespace tilegame::components
{
    struct Timer
    {
        float time_total;
        float time_left;
        bool repeat;

        Timer() = default;
        Timer(float time_total, bool repeat) : time_total(time_total), time_left(time_total), repeat(repeat) {}
    };

    struct TimerEvent
    {
        inline static const std::string EVENT_TYPE = "TIMER_EVENT";
        float duration;
        bool repeated;

        TimerEvent() {}
        TimerEvent(float duration, bool repeated) : duration(duration), repeated(repeated) {}
    };
} // namespace tilegame
