#pragma once

#include <string>

#include <entt/entity/registry.hpp>

namespace tilegame::components
{
    struct Timer
    {
        double time_total;
        double time_left;

        Timer() = default;
        Timer(double time_total) : time_total(time_total), time_left(time_total) {}
    };

    struct TimerEventArgs
    {
        inline static const std::string EVENT_TYPE = "timer";
        double duration;

        TimerEventArgs() {}
        TimerEventArgs(double duration) : duration(duration) {}
    };
} // namespace tilegame
