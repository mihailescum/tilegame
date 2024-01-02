local dbg = require("debugger")
local inspect = require("inspect")

local man1 = ...
local events = {}

local function handle_event(event_type, event, source)
    events[event_type](event, source)
end

local function handle_timer_event() 
    while true do
        print("Hello man!")
        coroutine.yield()
    end
end

local positions = {
    vec2(320, 288),
    vec2(354.0, 514),
    vec2(546.0, 546.0),
    vec2(512.0, 320.0),
}
local target = positions[1]
local target_component = _Target(target)
local speed_component = _Speed(300)
_registry:emplace(man1.entity, target_component)
_registry:emplace(man1.entity, speed_component)

local function handle_target_reached_event()
    local current_index = 1
    while true do
        current_index = current_index + 1
        if current_index > 4 then current_index = 1 end

        local new_target = positions[current_index]
        local target_component = _Target(new_target)
        _registry:emplace(man1.entity, target_component)
        _registry:emplace(man1.entity, speed_component)

        coroutine.yield()
    end
end

local timer1 = _registry:create()
local timer_component = _Timer(20, true)
_registry:emplace(timer1, timer_component)

_add_event_listener(_TimerEvent, handle_event, timer1)
events[_TimerEvent.EVENT_TYPE] = coroutine.wrap(handle_timer_event)

_add_event_listener(_TargetReachedEvent, handle_event, man1.entity)
events[_TargetReachedEvent.EVENT_TYPE] = coroutine.wrap(handle_target_reached_event)

print("Man loaded")

return man1