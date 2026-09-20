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
    print("Hello")
end

local timer1 = _registry:create()
local timer_component = _Timer(10, true)
_registry:emplace(timer1, timer_component)

_add_event_listener(_TimerEvent, coroutine.wrap(handle_timer_event), timer1)
_add_event_listener(_InteractEvent, handle_interact_event, entity)
_registry:emplace(entity, _Interactable())