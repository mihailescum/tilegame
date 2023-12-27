local dbg = require("debugger")
local inspect = require("inspect")

local man1 = ...
local events = {}

local function handle_event(event_type, event, source)
    events[event_type](soldier1, event, source)
end

local function handle_timer_event() 
    while true do
        print("Hello man!")
        coroutine.yield()
    end
end

local function handle_target_reached_event() 
    while true do
        print("Man reached the target!")
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
local velocity_component = _VelocityComponent.new(30)
_add_component(man1.entity, target_component)
_add_component(man1.entity, velocity_component)

_add_event_listener("TARGET_REACHED_EVENT", handle_event, man1.entity)
events["TARGET_REACHED_EVENT"] = coroutine.wrap(handle_target_reached_event)

return man1