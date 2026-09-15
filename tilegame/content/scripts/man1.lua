local dbg = require("debugger")
local inspect = require("inspect")

local man1 = ...

local function handle_timer_event(event_type, event, source) 
    while true do
        print("Hello man!")
        coroutine.yield()
    end
end

local positions = {
    _to_global("map1", vec2(320, 288)),
    _to_global("map1", vec2(354.0, 514)),
    _to_global("map1", vec2(546.0, 546.0)),
    _to_global("map1", vec2(512.0, 320.0)),
}
local target = positions[1]
local target_component = _Target(target)
local speed_component = _Speed(300)
_registry:emplace(man1.entity, target_component)
_registry:emplace(man1.entity, speed_component)

local function handle_target_reached_event(event_type, event, source)
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

_add_event_listener(_TimerEvent, coroutine.wrap(handle_timer_event), timer1)
_add_event_listener(_TargetReachedEvent, coroutine.wrap(handle_target_reached_event), man1.entity)

print("Man loaded")

-- Manual test for show_message: prints a message that should wrap across more
-- than two lines, blocking movement until the player has pressed Enter through
-- all of them.
_add_event_listener(_MessageClosedEvent, function(event_type, event, source)
    print("Message closed")
end)
_show_message("Hello! This is a longer test message that should wrap across more than two lines when the dialog box is on screen.")

return man1