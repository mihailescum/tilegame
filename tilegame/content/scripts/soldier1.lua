local soldier1 = {}

local function main()
    print("Script started")

    entity = _create_entity()
    c = _TimerComponent.new(3)
    _add_component(entity, c)
    -- coroutine.yield()
    -- soldier1:main()

    print("Timer finished")
end

soldier1.main = coroutine.wrap(main)

return soldier1