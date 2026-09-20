local map2_entity = ...

local function handle_map_entered_event(event_type, event, source)
    if event.map == map2_entity then
        print("Entered map2.")
    end
end

local function handle_map_left_event(event_type, event, source)
    if event.map == map2_entity then
        print("Left map2.")
    end
end

_add_event_listener(_MapEnteredEvent, handle_map_entered_event)
_add_event_listener(_MapLeftEvent, handle_map_left_event)