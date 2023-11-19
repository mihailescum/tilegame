local soldier1

local function handle_event(event_type, event)
    print("Handle event")

    soldier1[event_type](event)

    print("Handle event finished")
end

local function handle_timer_event(self, event) 
    print("Timer 1 finished!")
    coroutine.yield()
    print("Timer 2 finished!")
end

timer1 = _create_entity()
c1 = _TimerComponent.new(1)
_add_component(timer1, c1)

timer2 = _create_entity()
c2 = _TimerComponent.new(3)
_add_component(timer2, c2)

_add_timer_event_listener(handle_event, timer1)
_add_timer_event_listener(handle_event, timer2)


soldier1 = {
    handle_event = handle_event,
    timer = coroutine.wrap(handle_timer_event),
}
return soldier1