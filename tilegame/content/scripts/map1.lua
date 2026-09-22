local map1_entity = ...

local function handle_map_entered_event(event_type, event, source, target)
    print("Entered map1.")
end

local function handle_map_left_event(event_type, event, source, target)
    print("Left map1.")
end

-- _null_entity source: don't care which entity moved, only that it moved into/out of map1_entity.
_add_event_listener(_MapEnteredEvent, handle_map_entered_event, _null_entity, map1_entity)
_add_event_listener(_MapLeftEvent, handle_map_left_event, _null_entity, map1_entity)