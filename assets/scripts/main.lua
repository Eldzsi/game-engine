eventHandlers = {}

function addEventHandler(eventName, func)
    if not eventHandlers[eventName] then
        eventHandlers[eventName] = {}
    end

    table.insert(eventHandlers[eventName], func)
end

function triggerEvent(eventName)
    if eventHandlers[eventName] then
        for i, func in ipairs(eventHandlers[eventName]) do
            func()
        end
    end
end

dofile("assets/scripts/sky.lua")
dofile("assets/scripts/jump.lua")
dofile("assets/scripts/level1.lua")

