#pragma once

#include <string>

#include <entt/entity/registry.hpp>

namespace tilegame::components
{
    struct Timer
    {
        double time_total;
        double time_left;
        bool repeat;

        Timer() = default;
        Timer(double time_total, bool repeat) : time_total(time_total), time_left(time_total), repeat(repeat) {}
    };

    struct TimerEventArgs
    {
        inline static const std::string EVENT_TYPE = "timer";
        double duration;
        bool repeated;

        TimerEventArgs() {}
        TimerEventArgs(double duration, bool repeated) : duration(duration), repeated(repeated) {}
    };
} // namespace tilegame
