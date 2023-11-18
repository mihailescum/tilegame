#pragma once

namespace tilegame::components
{
    struct Timer
    {
        double time_total;
        double time_left;

        Timer() = default;
        Timer(double time_total) : time_total(time_total), time_left(time_total) {}
    };
} // namespace tilegame
