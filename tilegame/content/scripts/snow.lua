local map_entity = ...

local function handle_map_entered_event(event_type, event, source, target)
    local weather = require("weather")
    weather.start_snow()
end

local function handle_map_left_event(event_type, event, source, target)
    local weather = require("weather")
    weather.end_weather()
end

_add_event_listener(_MapEnteredEvent, handle_map_entered_event, player1_entity, map_entity)
_add_event_listener(_MapLeftEvent, handle_map_left_event, player1_entity, map_entity)
