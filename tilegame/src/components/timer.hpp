#pragma once

#include <string>

#include <entt/entity/registry.hpp>

#include "sol/sol.hpp"

namespace tilegame::components
{
    struct Timer
    {
        float time_total;
        float time_left;
        bool repeat;

        Timer() = default;
        Timer(float time_total, bool repeat) : time_total(time_total), time_left(time_total), repeat(repeat) {}

        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };

    struct TimerEvent
    {
        inline static const std::string EVENT_TYPE = "TIMER_EVENT";
        float duration;
        bool repeated;

        TimerEvent() = default;
        TimerEvent(float duration, bool repeated) : duration(duration), repeated(repeated) {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame
