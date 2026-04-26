activeResources = {}
local runningGamemode = nil 

local originalCursorState = _G.showCursor
local cursorStates = {}

_G.localPlayer = "local_player_entity"

_G.exports = {}

_G.showCursor = function(state, requester)
    requester = requester or "core"
    
    if state then
        cursorStates[requester] = true
    else
        cursorStates[requester] = nil
    end

    local shouldBeVisible = false
    for _, _ in pairs(cursorStates) do
        shouldBeVisible = true
        break
    end

    return originalCursorState(shouldBeVisible)
end

function isCursorVisible()
    for _, _ in pairs(cursorStates) do
        return true
    end
    
    return false
end

local function resolvePath(filepath, resourcePath)
    if type(filepath) ~= "string" then 
        return filepath 
    end

    if string.match(filepath, "^[/\\\\]") then
        return string.sub(filepath, 2)
    end

    return resourcePath .. filepath
end

function startResource(resourceName)
    if activeResources[resourceName] then
        print("WARNING: " .. resourceName .. " is already running.")
        return false
    end

    local resourcePath = "resources/" .. resourceName .. "/"
    local metaPath = resourcePath .. "meta.lua"
    
    local metaData = {
        name = resourceName,
        author = "Unknown",
        description = "",
        version = "1.0",
        type = "script",
        scripts = {},
        maps = {},
        exports = {},
        dependencies = {},
        thumbnail = nil
    }

    local metaEnvironment = {
        name = function(v)
            metaData.name = tostring(v) 
        end,
        author = function(v)
            metaData.author = tostring(v) 
        end,
        description = function(v)
            metaData.description = tostring(v)
        end,
        version = function(v)
            metaData.version = tostring(v)
        end,
        type = function(v)
            metaData.type = string.lower(tostring(v))
        end,
        thumbnail = function(v)
            metaData.thumbnail = tostring(v)
        end,
        scripts = function(t)
            if type(t) == "table" then
                for _, s in ipairs(t) do
                    table.insert(metaData.scripts, s)
                end
            end
        end,
        maps = function(t)
            if type(t) == "table" then
                for _, m in ipairs(t) do
                    table.insert(metaData.maps, m)
                end
            end
        end,
        exports = function(t)
            if type(t) == "table" then
                for _, e in ipairs(t) do
                    table.insert(metaData.exports, e)
                end
            end
        end,
        dependencies = function(t)
            if type(t) == "table" then
                for _, d in ipairs(t) do
                    table.insert(metaData.dependencies, d)
                end
            end
        end
    }

    local metaFunc, loadErr = loadfile(metaPath, "t", metaEnvironment)
    if not metaFunc then
        print("ERROR: Could not find meta.lua for resource " .. resourceName)
        return false
    end

    local success, runErr = pcall(metaFunc)
    if not success then
        print("ERROR: Invalid syntax in meta.lua of " .. resourceName .. ": " .. tostring(runErr))
        return false
    end

    local validTypes = {
        ["gamemode"] = true,
        ["script"]   = true
    }

    if not validTypes[metaData.type] then
        print("ERROR: Invalid resource type '" .. tostring(metaData.type) .. "' in resource '" .. resourceName .. "'. ")

        return false
    end

    for _, depName in ipairs(metaData.dependencies) do
        if not activeResources[depName] then
            print("INFO: Starting dependency '" .. depName .. "' for resource '" .. resourceName .. "'...")
            local depStarted = startResource(depName)
            if not depStarted then
                print("ERROR: Failed to start required dependency '" .. depName .. " for resource '" .. resourceName .. "'.")

                return false
            end
        end
    end

    if metaData.type == "gamemode" then
        if runningGamemode and runningGamemode ~= resourceName then
            print("INFO: Stopping current gamemode '" .. runningGamemode .. "' to start '" .. resourceName .. "'...")
            stopResource(runningGamemode)
        end
        runningGamemode = resourceName

        if _G.triggerEvent then
            _G.triggerEvent("onGamemodeStart", "root", resourceName)
        end
    end

    local resourceEnvironment = setmetatable({}, {__index = _G})
    
    activeResources[resourceName] = {
        meta = metaData,
        objects = {}, timers = {}, events = {},
        sounds = {}, pickups = {}, zones = {},
        fires = {}, lights = {}, hasTerrain = false,
        mapEntities = { objects = {}, pickups = {}, fires = {}, lights = {}, timers = {} }
    }

    resourceEnvironment.createObject = function(model, texture, ...)
        local finalModel = resolvePath(model, resourcePath)
        
        local finalTexture
        if type(texture) == "table" then
            finalTexture = {}
            for i, path in ipairs(texture) do
                finalTexture[i] = resolvePath(path, resourcePath)
            end
        else
            finalTexture = resolvePath(texture, resourcePath)
        end

        local id = _G.createObject(finalModel, finalTexture, ...)
        if id then
            table.insert(activeResources[resourceName].objects, id)
        end

        return id
    end

    resourceEnvironment.createPickup = function(model, texture, ...)
        local finalModel = resolvePath(model, resourcePath)
        local finalTexture = resolvePath(texture, resourcePath)
        local id = _G.createPickup(finalModel, finalTexture, ...)
        if id then
            table.insert(activeResources[resourceName].pickups, id)
        end

        return id
    end

    resourceEnvironment.loadTerrain = function(heightmap, texture, ...)
        local finalHeightmap = resolvePath(heightmap, resourcePath)
        local finalTexture = resolvePath(texture, resourcePath)
        local loaded = _G.loadTerrain(finalHeightmap, finalTexture, ...)
        if loaded then
            activeResources[resourceName].hasTerrain = true
        end

        return loaded
    end

    resourceEnvironment.playSound = function(filepath, ...)
        local finalPath = resolvePath(filepath, resourcePath)
        local channel = _G.playSound(finalPath, ...)
        if channel then
            table.insert(activeResources[resourceName].sounds, channel)
        end

        return channel
    end

    resourceEnvironment.playSound3D = function(filepath, ...)
        local finalPath = resolvePath(filepath, resourcePath)
        local channel = _G.playSound3D(finalPath, ...)
        if channel then
            table.insert(activeResources[resourceName].sounds, channel)
        end

        return channel
    end

    resourceEnvironment.drawImage = function(x, y, w, h, filepath, ...)
        local finalPath = resolvePath(filepath, resourcePath)

        return _G.drawImage(x, y, w, h, finalPath, ...)
    end

    resourceEnvironment.drawText = function(text, x, y, font, size, ...)
        local finalFont = resolvePath(font, resourcePath)

        return _G.drawText(text, x, y, finalFont, size, ...)
    end

    resourceEnvironment.showCursor = function(state)
        return _G.showCursor(state, resourceName)
    end

    resourceEnvironment.createSpotLight = function(...)
        local id = _G.createSpotLight(...)
        if id and id ~= -1 then
            table.insert(activeResources[resourceName].lights, {id = id, type = "spot"})
        end

        return id
    end

    resourceEnvironment.createPointLight = function(...)
        local id = _G.createPointLight(...)
        if id and id ~= -1 then
            table.insert(activeResources[resourceName].lights, {id = id, type = "point"})
        end

        return id
    end

    resourceEnvironment.setTimer = function(...)
        local timer = _G.setTimer(...)
        if timer then
            table.insert(activeResources[resourceName].timers, timer)
        end

        return timer
    end

    resourceEnvironment.addEventHandler = function(eventName, attachedTo, handler)
        if type(attachedTo) == "function" then
            handler = attachedTo
            attachedTo = "root"
        end

        _G.addEventHandler(eventName, attachedTo, handler)

        table.insert(activeResources[resourceName].events, {
            name = eventName,
            attachedTo = attachedTo,
            handler = handler
        })
    end

    resourceEnvironment.createZoneSphere = function(...)
        local id = _G.createZoneSphere(...)
        if id then
            table.insert(activeResources[resourceName].zones, id)
        end

        return id
    end

    resourceEnvironment.createFire = function(...)
        local id = _G.createFire(...)
        if id then
            table.insert(activeResources[resourceName].fires, id)
        end

        return id
    end

    local mapEnvironment = {
        object = function(data)
            local x, y, z = table.unpack(data.position or {0, 0, 0})
            local rx, ry, rz = table.unpack(data.rotation or {0, 0, 0})
            local sx, sy, sz = table.unpack(data.scale or {1, 1, 1})
            local id = resourceEnvironment.createObject(data.model, data.texture, x, y, z, rx, ry, rz, sx, sy, sz)
            
            if id then
                table.insert(activeResources[resourceName].mapEntities.objects, id)
                if data.glow then
                    _G.setObjectGlow(id, true, table.unpack(data.glow))
                end
                if data.collisions ~= nil then
                    _G.setElementCollisionsEnabled(id, data.collisions)
                end
                if data.uvSpeed then
                    _G.setObjectUVSpeed(id, table.unpack(data.uvSpeed))
                end
            end
        end,
        
        movingObject = function(data)
            local x, y, z = table.unpack(data.position or {0, 0, 0})
            local rx, ry, rz = table.unpack(data.rotation or {0, 0, 0})
            local sx, sy, sz = table.unpack(data.scale or {1, 1, 1})
            local id = resourceEnvironment.createObject(data.model, data.texture, x, y, z, rx, ry, rz, sx, sy, sz)
            
            if id then
                table.insert(activeResources[resourceName].mapEntities.objects, id)
                if data.glow then
                    _G.setObjectGlow(id, true, table.unpack(data.glow))
                end
                if data.collisions ~= nil then
                    _G.setElementCollisionsEnabled(id, data.collisions)
                end
                if data.uvSpeed then
                    _G.setObjectUVSpeed(id, table.unpack(data.uvSpeed))
                end
            end

            if id and (data.target or data.targetRotation) then
                local tx, ty, tz = table.unpack(data.target or {x, y, z})
                local trx, try, trz = table.unpack(data.targetRotation or {rx, ry, rz})
                local duration = data.duration or 2000
                
                if data.pingpong or data.loop then
                    _G.moveObject(id, duration, tx, ty, tz, trx, try, trz)
                    local toTarget = false
                    
                    local currentTrx, currentTry, currentTrz = trx, try, trz
                    
                    local timerId = resourceEnvironment.setTimer(function()
                        if data.loop then
                            currentTrx = currentTrx + (trx - rx)
                            currentTry = currentTry + (try - ry)
                            currentTrz = currentTrz + (trz - rz)
                            _G.moveObject(id, duration, tx, ty, tz, currentTrx, currentTry, currentTrz)
                            
                        elseif data.pingpong then
                            if toTarget then
                                _G.moveObject(id, duration, tx, ty, tz, trx, try, trz)
                            else
                                _G.moveObject(id, duration, x, y, z, rx, ry, rz)
                            end
                            toTarget = not toTarget
                        end
                    end, duration, 0)
                    
                    if timerId then
                        table.insert(activeResources[resourceName].mapEntities.timers, timerId)
                    end
                elseif data.autoStart then
                    _G.moveObject(id, duration, tx, ty, tz, trx, try, trz)
                else
                    _G.setElementData(id, "targetPos", {tx, ty, tz})
                    _G.setElementData(id, "targetRot", {trx, try, trz})
                    _G.setElementData(id, "moveDuration", duration)
                end
            end
        end,
        
        pickup = function(data)
            local x, y, z = table.unpack(data.position or {0, 0, 0})
            local id = resourceEnvironment.createPickup(data.model, data.texture, x, y, z, data.type)
            if id then
                table.insert(activeResources[resourceName].mapEntities.pickups, id)
            end
        end,
        
        fire = function(data)
            local x, y, z = table.unpack(data.position or {0, 0, 0})
            local id = resourceEnvironment.createFire(x, y, z, data.density, data.scale)
            if id then
                table.insert(activeResources[resourceName].mapEntities.fires, id)
            end
        end,
        
        pointlight = function(data)
            local x, y, z = table.unpack(data.position or {0, 0, 0})
            local r, g, b = table.unpack(data.color or {1, 1, 1})
            local id = resourceEnvironment.createPointLight(x, y, z, r, g, b, data.constant or 1.0, data.linear or 0.09, data.quadratic or 0.032)
            if id then
                table.insert(activeResources[resourceName].mapEntities.lights, {id = id, type = "point"})
            end
        end,
        
        spotlight = function(data)
            local x, y, z = table.unpack(data.position or {0, 0, 0})
            local dx, dy, dz = table.unpack(data.direction or {0, -1, 0})
            local r, g, b = table.unpack(data.color or {1, 1, 1})
            local id = resourceEnvironment.createSpotLight(x, y, z, dx, dy, dz, r, g, b, data.cutoff or 12.5, data.outerCutoff or 15.0, data.constant or 1.0, data.linear or 0.09, data.quadratic or 0.032)
            if id then
                table.insert(activeResources[resourceName].mapEntities.lights, {id = id, type = "spot"})
            end
        end
    }

    resourceEnvironment.reloadMap = function()
        local res = activeResources[resourceName]
        if not res then return false end

        local function removeFromList(list, val)
            for i = #list, 1, -1 do
                if list[i] == val then
                    table.remove(list, i)
                end
            end
        end

        local function removeFromLightList(list, id)
            for i = #list, 1, -1 do
                if list[i].id == id then
                    table.remove(list, i)
                end
            end
        end

        for _, id in ipairs(res.mapEntities.objects) do
            _G.destroyElement(id)
            removeFromList(res.objects, id)
        end
        for _, id in ipairs(res.mapEntities.pickups) do
            if _G.destroyPickup then 
                _G.destroyPickup(id)
            end
            removeFromList(res.pickups, id)
        end

        for _, id in ipairs(res.mapEntities.fires) do
            if _G.destroyFire then
                _G.destroyFire(id)
            end
            removeFromList(res.fires, id) end

        for _, light in ipairs(res.mapEntities.lights) do 
            if light.type == "spot" then
                if _G.destroySpotLight then
                    _G.destroySpotLight(light.id)
                end
            elseif _G.destroyPointLight then
                _G.destroyPointLight(light.id)
            end
            removeFromLightList(res.lights, light.id)
        end

        for _, timer in ipairs(res.mapEntities.timers) do
            if _G.destroyTimer then
                _G.destroyTimer(timer)
            end
            removeFromList(res.timers, timer)
        end

        res.mapEntities = {objects = {}, pickups = {}, fires = {}, lights = {}, timers = {}}

        for _, mapName in ipairs(metaData.maps) do
            local mapPath = resourcePath .. mapName
            local mapChunk, err = loadfile(mapPath, "t", mapEnvironment)
            if mapChunk then
                pcall(mapChunk)
            end
        end

        print("INFO: Map reloaded for resource '" .. resourceName .. "'")

        return true
    end

    for _, mapName in ipairs(metaData.maps) do
        local mapPath = resourcePath .. mapName
        local mapChunk, err = loadfile(mapPath, "t", mapEnvironment)
        
        if not mapChunk then
            print("ERROR: Error loading map '" .. mapName .. "': " .. tostring(err))
        else
            local success, runErr = pcall(mapChunk)
            if not success then
                print("ERROR: Syntax error in map '" .. mapName .. "': " .. tostring(runErr))
            end
        end
    end

    for _, scriptName in ipairs(metaData.scripts) do
        local scriptPath = resourcePath .. scriptName
        local chunk, err = loadfile(scriptPath, "t", resourceEnvironment)
        
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

    if #metaData.exports > 0 then
        _G.exports[resourceName] = {}
        for _, funcName in ipairs(metaData.exports) do
            if type(resourceEnvironment[funcName]) == "function" then
                _G.exports[resourceName][funcName] = resourceEnvironment[funcName]
            else
                print("ERROR: Exported function '" .. funcName .. "' not found in resource '" .. resourceName .. "'")
            end
        end
    end

    print("INFO: Resource '" .. resourceName .. "' started (" .. metaData.type .. ").")
    return true
end

function stopResource(resourceName)
    local res = activeResources[resourceName]
    if not res then
        return false
    end

    if res.meta.type == "gamemode" and runningGamemode == resourceName then
        runningGamemode = nil

        if _G.triggerEvent then
            _G.triggerEvent("onGamemodeStop", "root", resourceName)
        end
    end

    for _, id in ipairs(res.objects) do
        _G.destroyElement(id)
    end

    for _, timer in ipairs(res.timers) do
        if _G.destroyTimer then
            _G.destroyTimer(timer)
        end
    end

    for _, evt in ipairs(res.events) do
        _G.removeEventHandler(evt.name, evt.attachedTo, evt.handler)
    end

    for _, channel in ipairs(res.sounds) do
        if _G.stopSound then
            _G.stopSound(channel)
        end
    end

    for _, id in ipairs(res.pickups) do
        if _G.destroyPickup then
            _G.destroyPickup(id)
        end
    end

    for _, id in ipairs(res.zones) do
        if _G.destroyZone then
            _G.destroyZone(id)
        end
    end

    for _, id in ipairs(res.fires) do
        if _G.destroyFire then
            _G.destroyFire(id)
        end
    end

    for _, light in ipairs(res.lights) do
        if light.type == "spot" then
            if _G.destroySpotLight then
                _G.destroySpotLight(light.id)
            end
        else
            if _G.destroyPointLight then
                _G.destroyPointLight(light.id)
            end
        end
    end
    if res.hasTerrain then
        if _G.unloadTerrain then
            _G.unloadTerrain()
        end
    end

    _G.showCursor(false, resourceName)

    if _G.exports[resourceName] then
        _G.exports[resourceName] = nil
    end

    if isGamemode then
        if _G.clearEngineCaches then
            _G.clearEngineCaches()
        end
        collectgarbage("collect")
    end

    activeResources[resourceName] = nil
    print("INFO: Resource '" .. resourceName .. "' stopped.")

    return true
end

function restartResource(resourceName)
    stopResource(resourceName)
    startResource(resourceName)
end

function getResourceInfo(resourceName)
    local resourcePath = "resources/" .. resourceName .. "/"
    local metaPath = resourcePath .. "meta.lua"
    
    local metaData = {
        name = resourceName,
        author = "Unknown",
        description = "-",
        version = "1.0",
        type = "script",
        thumbnail = nil
    }

    local metaEnvironment = {
        name = function(v)
            metaData.name = tostring(v)
        end,
        author = function(v)
            metaData.author = tostring(v)
        end,
        description = function(v)
            metaData.description = tostring(v)
        end,
        version = function(v)
            metaData.version = tostring(v)
        end,
        type = function(v)
            metaData.type = string.lower(tostring(v))
        end,
        thumbnail = function(v)
            metaData.thumbnail = tostring(v)
        end,
        scripts = function()
        end,
        maps = function()
        end,
        dependencies = function() 
        end,
        exports = function()
        end
    }

    local fn, err = loadfile(metaPath, "t", metaEnvironment)
    if fn then
        pcall(fn)

        if metaData.thumbnail then
            metaData.thumbnail = "/" .. resourcePath .. metaData.thumbnail
        end

        return metaData
    end
    
    return nil

end

function getRunningGamemode()
    return runningGamemode
end
