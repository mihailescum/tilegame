local soldier1

local function handle_event(event_type, event, source)
    soldier1[event_type](soldier1, event, source)
end

local function handle_timer_event(self, event, source) 
    while true do
        print("Timer trigger", event.duration, source)
        self, event, source = coroutine.yield()
    end
end

timer1 = _create_entity()
c1 = _TimerComponent.new(1, true)
_add_component(timer1, c1)

timer2 = _create_entity()
c2 = _TimerComponent.new(2, true)
_add_component(timer2, c2)

_add_timer_event_listener(handle_event, timer1)
_add_timer_event_listener(handle_event, timer2)


soldier1 = {
    handle_event = handle_event,
    timer = coroutine.wrap(handle_timer_event),
}
return soldier1