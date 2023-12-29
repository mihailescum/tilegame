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
    vec2.new(32.0, 32.0),
    vec2.new(64.0, 128.0),
    vec2.new(512.0, 256.0),
    vec2.new(256.0, 48.0),
    vec2.new(128.0, 128.0)
}

local function handle_target_reached_event() 
    local current_index = -1
    while true do
        print("Man reached the target!")

        local new_index = -1
        repeat
            new_index = math.random(1,5)
        until new_index ~= current_index
        
        current_index = new_index

        local new_target = positions[current_index]
        local target_component = _TargetComponent.new(new_target)
        local velocity_component = _VelocityComponent.new(300)
        _add_component(man1.entity, target_component)
        _add_component(man1.entity, velocity_component)

        coroutine.yield()
    end
end

local timer1 = _create_entity()
local timer_component = _TimerComponent.new(20, true)
_add_component(timer1, timer_component)

_add_event_listener("TIMER_EVENT", handle_event, timer1)
events["TIMER_EVENT"] = coroutine.wrap(handle_timer_event)

local target = vec2.new(16.0, 16.0)
local target_component = _TargetComponent.new(target)
local velocity_component = _VelocityComponent.new(300)
_add_component(man1.entity, target_component)
_add_component(man1.entity, velocity_component)

_add_event_listener("TARGET_REACHED_EVENT", handle_event, man1.entity)
events["TARGET_REACHED_EVENT"] = coroutine.wrap(handle_target_reached_event)

print("Man loaded")

return man1