local man1

a, b, c = ...
print(a, b, c)

local timer1 = _create_entity()
local c1 = _TimerComponent.new(2, true)
_add_component(timer1, c1)

_add_timer_event_listener(
    function() print("Hello man!") end, timer1)

man1 = {
    -- handle_event = handle_event,
    -- timer = coroutine.wrap(handle_timer_event),
}
return man1