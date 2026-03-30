activeResources = {}

function startResource(resourceName)
    if activeResources[resourceName] then
        print("WARNING: " .. resourceName .. " is already running!")
        return false
    end

    local resourcePath = "resources/" .. resourceName .. "/"
    local metaPath = resourcePath .. "meta.lua"
    
    local metaFunc, loadErr = loadfile(metaPath)
    if not metaFunc then
        print("ERROR: Could not find meta.lua for resource " .. resourceName)
        return false
    end

    local metaData = metaFunc()
    if type(metaData) ~= "table" or not metaData.scripts then
        print("ERROR: Invalid meta.lua in " .. resourceName)
        return false
    end

    local resEnv = setmetatable({}, {__index = _G})
    
    activeResources[resourceName] = {
        objects = {}, timers = {}, events = {},
        sounds = {}, pickups = {}, zones = {},
        fires = {}, lights = {}
    }

    resEnv.createObject = function(...)
        local id = _G.createObject(...)
        if id then table.insert(activeResources[resourceName].objects, id) end
        return id
    end

    resEnv.createSpotLight = function(...)
        local id = _G.createSpotLight(...)
        if id and id ~= -1 then 
            table.insert(activeResources[resourceName].lights, {id = id, type = "spot"}) 
        end
        return id
    end

    resEnv.createPointLight = function(...)
        local id = _G.createPointLight(...)
        if id and id ~= -1 then 
            table.insert(activeResources[resourceName].lights, {id = id, type = "point"}) 
        end
        return id
    end

    resEnv.setTimer = function(...)
        local timer = _G.setTimer(...)
        if timer then table.insert(activeResources[resourceName].timers, timer) end
        return timer
    end

    resEnv.addEventHandler = function(eventName, arg2, arg3)
        _G.addEventHandler(eventName, arg2, arg3)
        local func = type(arg2) == "function" and arg2 or arg3
        local attachedTo = type(arg2) == "function" and "root" or arg2
        table.insert(activeResources[resourceName].events, {name = eventName, attachedTo = attachedTo, handler = func})
    end

    resEnv.createPickup = function(...)
        local id = _G.createPickup(...)
        if id then table.insert(activeResources[resourceName].pickups, id) end
        return id
    end

    resEnv.createZoneSphere = function(...)
        local id = _G.createZoneSphere(...)
        if id then table.insert(activeResources[resourceName].zones, id) end
        return id
    end

    resEnv.playSound = function(filepath, ...)
        local finalPath = filepath
        if not string.match(filepath, "^assets/") then
            finalPath = resourcePath .. filepath
        end
        local channel = _G.playSound(finalPath, ...)
        if channel then table.insert(activeResources[resourceName].sounds, channel) end
        return channel
    end

    resEnv.playSound3D = function(filepath, ...)
        local finalPath = filepath
        if not string.match(filepath, "^assets/") then
            finalPath = resourcePath .. filepath
        end
        local channel = _G.playSound3D(finalPath, ...)
        if channel then table.insert(activeResources[resourceName].sounds, channel) end
        return channel
    end

    resEnv.createFire = function(...)
        local id = _G.createFire(...)
        if id then table.insert(activeResources[resourceName].fires, id) end
        return id
    end

    for _, scriptName in ipairs(metaData.scripts) do
        local scriptPath = resourcePath .. scriptName
        local chunk, err = loadfile(scriptPath, "t", resEnv)
        
        if not chunk then
            print("ERROR: Error loading script '" .. scriptName .. "' in resource " .. resourceName .. ": " .. tostring(err))
            stopResource(resourceName)
            return false
        end

        local success, runErr = pcall(chunk)
        if not success then
            print("ERROR: Error running script " .. scriptName .. " in resource '" .. resourceName .. "': " .. tostring(runErr))
            stopResource(resourceName)
            return false
        end
    end

    print("INFO: Resource " .. resourceName .. " started.")
    return true
end

function stopResource(resourceName)
    local res = activeResources[resourceName]
    if not res then return false end

    for _, id in ipairs(res.objects) do
        _G.destroyElement(id)
    end

    for _, timer in ipairs(res.timers) do
        if _G.killTimer then _G.killTimer(timer) end
    end

    for _, evt in ipairs(res.events) do
        _G.removeEventHandler(evt.name, evt.attachedTo, evt.handler)
    end

    for _, channel in ipairs(res.sounds) do
        if _G.stopSound then _G.stopSound(channel) end
    end

    for _, id in ipairs(res.pickups) do
        if _G.destroyPickup then _G.destroyPickup(id) end
    end

    for _, id in ipairs(res.zones) do
        if _G.destroyZone then _G.destroyZone(id) end
    end

    for _, id in ipairs(res.fires) do
        if _G.destroyFire then _G.destroyFire(id) end
    end

    for _, light in ipairs(res.lights) do
        if light.type == "spot" then
            if _G.destroySpotLight then _G.destroySpotLight(light.id) end
        else
            if _G.destroyPointLight then _G.destroyPointLight(light.id) end
        end
    end

    activeResources[resourceName] = nil
    print("INFO: Resource " .. resourceName .. " stopped.")
    return true
end

function restartResource(resourceName)
    stopResource(resourceName)
    startResource(resourceName)
end
