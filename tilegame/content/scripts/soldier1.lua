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

local timer1 = _registry:create()
local timer_component = _Timer(10, true)
_registry:emplace(timer1, timer_component)

_add_event_listener(_TimerEvent, handle_event, timer1)
events[_TimerEvent.EVENT_TYPE] = coroutine.wrap(handle_timer_event)

return soldier1