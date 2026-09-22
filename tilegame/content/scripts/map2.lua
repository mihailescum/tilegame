local map2_entity = ...

local function handle_map_entered_event(event_type, event, source, target)
    print("Entered map2.")
end

local function handle_map_left_event(event_type, event, source, target)
    print("Left map2.")
end

-- _null_entity source: don't care which entity moved, only that it moved into/out of map2_entity.
_add_event_listener(_MapEnteredEvent, handle_map_entered_event, _null_entity, map2_entity)
_add_event_listener(_MapLeftEvent, handle_map_left_event, _null_entity, map2_entity)