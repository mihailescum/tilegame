local dbg = require("debugger")
local inspect = require("inspect")

local man1 = ...

local timer1 = _create_entity()
local timer_component = _TimerComponent.new(20, true)
_add_component(timer1, timer_component)

_add_timer_event_listener(function() print("Hello man!") end, timer1)

local target = vec2.new(16.0, 16.0)
local target_component = _TargetComponent.new(target)
local velocity_component = _VelocityComponent.new(100)
_add_component(man1.entity, target_component)
_add_component(man1.entity, velocity_component)

return man1