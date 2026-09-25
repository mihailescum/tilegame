local entity = ...

local function handle_timer_event(event_type, event, source)
    local count = 1
    while true do
        print("Soldier! Count:", count)
        count = count + 1
        event_type, event, source = coroutine.yield()
    end
end

local function handle_interact_event(event_type, event, source)
    _show_message("Hello!\nI'm a magician...\nWhich action do you want me to do?", false, {"Earthquake", "Stop the rain"})
    _stop_player_input(1)
    _add_event_listener(_MessageClosedEvent, function(event_type, event, source)
    print("Selected option: " .. event.selected_option)
    _resume_player_input(1)
end)
end

-- local timer1 = _registry:create()
-- local timer_component = _Timer(10, true)
-- _registry:emplace(timer1, timer_component)
-- _add_event_listener(_TimerEvent, coroutine.wrap(handle_timer_event), timer1)

_registry:emplace(entity, _Interactable())
_add_event_listener(_InteractEvent, handle_interact_event, entity)