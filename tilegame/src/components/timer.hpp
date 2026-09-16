#pragma once

#include <string>

#include "entt/entt.hpp"

#include "sol/sol.hpp"

namespace tilegame::components
{
    /**
     * @brief Countdown that raises a TimerEvent when it expires. The Timer system decrements
     * `time_left` each frame; if `repeat` is set it resets to `time_total` and keeps counting down,
     * otherwise the component is removed after firing. Exposed to Lua as `_Timer`.
     */
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

    /**
     * @brief Raised immediately by the Timer system (via System::raise_event()) when a Timer
     * expires, and delivered synchronously to every `EventListener<TimerEvent>`. A plain value
     * type - never stored as an entt component - that only exists for the duration of that
     * call. Exposed to Lua as `_TimerEvent`.
     */
    struct TimerEvent
    {
        inline static const std::string EVENT_TYPE = "TIMER_EVENT";
        /// Copy of the Timer's total duration at the time it fired.
        float duration;
        /// Whether the Timer that raised this event is set to repeat.
        bool repeated;

        TimerEvent() = default;
        TimerEvent(float duration, bool repeated) : duration(duration), repeated(repeated) {}
        [[nodiscard]] std::string to_string() const;

        static void register_component(sol::state &lua);
    };
} // namespace tilegame
