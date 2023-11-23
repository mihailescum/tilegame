local dbg = require("debugger")

local man1 = {} --...

dbg()

local timer1 = _create_entity()
local c1 = _TimerComponent.new(2, true)
_add_component(timer1, c1)

_add_timer_event_listener(
    function() print("Hello man!") end, timer1)

return man1