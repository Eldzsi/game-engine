local activePickups = {}
local zoneToPickup = {} 

function createPickup(model, texture, x, y, z, pickupType)
    local objId = createObject(model, texture, x, y, z, 0, 0, 0, 0.2, 0.2, 0.2)
    setElementCollisionsEnabled(objId, false) 
    
    setElementProperty(objId, "baseZ", z)
    if pickupType then
        setElementProperty(objId, "type", pickupType)
    end

    local zoneId = createZoneSphere(x, y, z, 1.2)
    zoneToPickup[zoneId] = objId

    table.insert(activePickups, {objId = objId, zoneId = zoneId})

    return objId
end

local globalTime = 0

addEventHandler("onRender", function(dt)
    dt = dt or 0.016
    globalTime = globalTime + dt

    local rotationSpeed = 100.0 * dt
    local floatAmplitude = 0.15 
    local floatSpeed = 3.0      

    for i = #activePickups, 1, -1 do
        local p = activePickups[i]
        local id = p.objId
        
        local rx, ry, rz = getElementRotation(id)
        if rx then
            setElementRotation(id, rx, ry, rz + rotationSpeed)

            local baseZ = getElementProperty(id, "baseZ") or 0
            local newZ = baseZ + math.sin(globalTime * floatSpeed) * floatAmplitude
            
            local x, y, z = getElementPosition(id)
            setElementPosition(id, x, y, newZ)
        else
            destroyZone(p.zoneId)
            zoneToPickup[p.zoneId] = nil
            table.remove(activePickups, i)
        end
    end
end)

addEventHandler("onColShapeHit", function(zoneId)
    local pickupId = zoneToPickup[zoneId]
    if pickupId then
        triggerEvent("onPickupHit", pickupId, pickupId) 
    end
end)

function destroyPickup(id)
    for i = #activePickups, 1, -1 do
        if activePickups[i].objId == id then
            local p = activePickups[i]
            
            destroyZone(p.zoneId) 
            zoneToPickup[p.zoneId] = nil
            
            destroyElement(id) 
            table.remove(activePickups, i)
            return true
        end
    end
    
    return false
end
