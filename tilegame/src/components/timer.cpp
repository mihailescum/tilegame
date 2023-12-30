#include "timer.hpp"

#include <entt/core/type_info.hpp>

#include "entt_sol/bond.hpp"

namespace tilegame::components
{
    std::string Timer::to_string() const
    {
        std::stringstream ss;
        ss << "Timer";
        return ss.str();
    }

    void Timer::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<Timer>();

        lua.new_usertype<Timer>(
            "_Timer",
            "type_id", &entt::type_hash<Timer>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return Timer(); },
                [](float time_total, bool repeat)
                { return Timer(time_total, repeat); }),
            "repeat", &components::Timer::repeat,
            "time_left", &Timer::time_left,
            "time_total", &Timer::time_total,
            sol::meta_function::to_string, &Timer::to_string);
    }

    std::string TimerEvent::to_string() const
    {
        std::stringstream ss;
        ss << "TimerEvent";
        return ss.str();
    }

    void TimerEvent::register_component(sol::state &lua)
    {
        entt_sol::register_meta_component<TimerEvent>();

        lua.new_usertype<TimerEvent>(
            "_TimerEvent",
            "type_id", &entt::type_hash<TimerEvent>::value,
            sol::call_constructor,
            sol::factories(
                []()
                { return TimerEvent(); },
                [](float time_total, bool repeat)
                { return TimerEvent(time_total, repeat); }),
            "duration", &components::TimerEvent::duration,
            "repeated", &TimerEvent::repeated,
            "EVENT_TYPE", sol::var(TimerEvent::EVENT_TYPE.c_str()),
            sol::meta_function::to_string, &TimerEvent::to_string);
    }
} // namespace tilegame::components
