local entity = ...
local weather = require("weather")

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
        _resume_player_input(1)
        if event.selected_option == 1 then
            _stop_player_input(1)
            local earthquake = _shake_camera({
                horizontal = { displacement_speed = 500, offset = 20, duration = 5 },
                vertical = { displacement_speed = 500, offset = 10, duration = 3 },
            })
            -- Filtered on the earthquake's entity, so e.g. a thunder rumble ending first doesn't
            -- resume input early.
            local listener
            listener = _add_event_listener(_CameraShakeEndedEvent, function(event_type, event, source)
                _resume_player_input(1)
                _remove_event_listener(listener)
            end, earthquake)
        else
            weather.end_weather()
        end
    end)
end

-- local timer1 = _registry:create()
-- local timer_component = _Timer(10, true)
-- _registry:emplace(timer1, timer_component)
-- _add_event_listener(_TimerEvent, coroutine.wrap(handle_timer_event), timer1)

_registry:emplace(entity, _Interactable(96, math.cos(math.pi / 3)))
_add_event_listener(_InteractEvent, handle_interact_event, entity)