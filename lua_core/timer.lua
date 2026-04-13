local activeTimers = {}

function setTimer(theFunction, timeInterval, timesToExecute, ...)
    local intervalSec = timeInterval / 1000.0 
    local maxExecutions = timesToExecute
    
    if maxExecutions == 0 then 
        maxExecutions = math.huge
    end

    local newTimer = {
        func = theFunction,
        interval = intervalSec,
        executeTimes = maxExecutions,
        executions = 0,
        elapsed = 0,
        args = {...}
    }
    
    table.insert(activeTimers, newTimer)
    
    return newTimer 
end

local function processTimers(dt)
    dt = dt or 0.016

    for i = #activeTimers, 1, -1 do
        local timer = activeTimers[i]
        timer.elapsed = timer.elapsed + dt 
        
        if timer.elapsed >= timer.interval then
            timer.func(table.unpack(timer.args)) 
            timer.elapsed = 0
            timer.executions = timer.executions + 1
            
            if timer.executions >= timer.executeTimes then
                table.remove(activeTimers, i)
            end
        end
    end
end
addEventHandler("onRender", processTimers)

function clearAllTimers()
    activeTimers = {}
end

function destroyTimer(theTimer)
    for i = #activeTimers, 1, -1 do
        if activeTimers[i] == theTimer then
            table.remove(activeTimers, i)
            return true
        end
    end

    return false
end
