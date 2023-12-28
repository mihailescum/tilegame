local soldier1 = ...

local events = {}

local function handle_event(event_type, event, source)
    events[event_type](event, source)
end

local function handle_timer_event(event, source) 
    local count = 1
    while true do
        print("Soldier! Count:", count)
        count = count + 1
        event, source = coroutine.yield()
    end
end

local timer1 = _create_entity()
local timer_component = _TimerComponent.new(10, true)
_add_component(timer1, timer_component)

_add_event_listener("TIMER_EVENT", handle_event, timer1)
events["TIMER_EVENT"] = coroutine.wrap(handle_timer_event)

return soldier1