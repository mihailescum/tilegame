local soldier1 = ...

local function handle_timer_event(event_type, event, source) 
    local count = 1
    while true do
        print("Soldier! Count:", count)
        count = count + 1
        event_type, event, source = coroutine.yield()
    end
end

local function handle_map_entered_event(event_type, event, source)
    map_name = _registry:get(source, _MapEnteredEvent).map_name
    print("Map entered: " .. map_name)
end

local function handle_map_left_event(event_type, event, source) 
    map_name = event.map_name
    print("Map left: " .. map_name)
end

local timer1 = _registry:create()
local timer_component = _Timer(10, true)
_registry:emplace(timer1, timer_component)

_add_event_listener(_TimerEvent, coroutine.wrap(handle_timer_event), timer1)
_add_event_listener(_MapEnteredEvent, handle_map_entered_event)
_add_event_listener(_MapLeftEvent, handle_map_left_event)

return soldier1