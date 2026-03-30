eventHandlers = {}

function addEventHandler(eventName, arg2, arg3)
    local attachedTo = "root"
    local func = nil

    if type(arg2) == "function" then
        func = arg2
    else
        attachedTo = arg2
        func = arg3
    end

    if not eventHandlers[eventName] then
        eventHandlers[eventName] = {}
    end

    table.insert(eventHandlers[eventName], {attachedTo = attachedTo, handler = func})
end

function triggerEvent(eventName, sourceElement, ...)
    if eventHandlers[eventName] then
        for i, evt in ipairs(eventHandlers[eventName]) do
            if evt.attachedTo == "root" or evt.attachedTo == sourceElement then
                source = sourceElement 
                evt.handler(...) 
            end
        end
    end
end

function removeEventHandler(eventName, arg2, arg3)
    local attachedTo = "root"
    local func = nil

    if type(arg2) == "function" then
        func = arg2
    else
        attachedTo = arg2
        func = arg3
    end

    if eventHandlers[eventName] then
        for i = #eventHandlers[eventName], 1, -1 do
            local evt = eventHandlers[eventName][i]
            if evt.attachedTo == attachedTo and evt.handler == func then
                table.remove(eventHandlers[eventName], i)
            end
        end
    end
end
