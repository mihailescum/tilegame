local dbg = require("debugger")
local inspect = require("inspect")

local entity = ...

local function handle_timer_event(event_type, event, source)
    while true do
        print("Hello man!")
        coroutine.yield()
    end
end

-- map1's world position is (480, 320) per content/worlds/world1.world. _to_global now takes the
-- map's entity rather than its name (see systems::World), and this script has no way to look
-- that entity up by name, so its offset is baked in here for now instead.
local map1_position = vec2(480, 320)
local positions = {
    map1_position + vec2(320, 288),
    map1_position + vec2(354.0, 514),
    map1_position + vec2(546.0, 546.0),
    map1_position + vec2(512.0, 320.0),
}
local target = positions[1]
local target_component = _Target(target)
local speed_component = _Speed(300)

_registry:emplace(entity, target_component)
_registry:emplace(entity, speed_component)

local function handle_target_reached_event(event_type, event, source)
    local current_index = 1
    while true do
        current_index = current_index + 1
        if current_index > 4 then current_index = 1 end

        local new_target = positions[current_index]
        local target_component = _Target(new_target)
        _registry:emplace(source, target_component)
        _registry:emplace(source, speed_component)

        coroutine.yield()
    end
end

local timer1 = _registry:create()
local timer_component = _Timer(20, true)
_registry:emplace(timer1, timer_component)

_add_event_listener(_TimerEvent, coroutine.wrap(handle_timer_event), timer1)
_add_event_listener(_TargetReachedEvent, coroutine.wrap(handle_target_reached_event), entity)

print("Man loaded")

-- Manual test for show_message: prints a message that should wrap across more
-- than two lines, blocking movement until the player has pressed Enter through
-- all of them.

_show_message("Hello! This is a longer test message that should wrap across more than two lines when the dialog box is on screen.", false, {"yes", "no"})
_stop_player_input(1)
_add_event_listener(_MessageClosedEvent, function(event_type, event, source)
    print("Selected option: " .. event.selected_option)
    _resume_player_input(1)
end)