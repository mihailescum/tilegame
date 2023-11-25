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

    struct TimerEventArgs
    {
        inline static const std::string EVENT_TYPE = "timer";
        float duration;
        bool repeated;

        TimerEventArgs() {}
        TimerEventArgs(float duration, bool repeated) : duration(duration), repeated(repeated) {}
    };
} // namespace tilegame
