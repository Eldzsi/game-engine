local zones = {}
local nextZoneID = 1

local function getDistanceBetweenPoints3D(x1, y1, z1, x2, y2, z2)
    local dx = x1 - x2
    local dy = y1 - y2
    local dz = z1 - z2
    
    return math.sqrt(dx*dx + dy*dy + dz*dz)
end

local function isPointInZone(px, py, pz, zone)
    if zone.type == "sphere" then
        local dist = getDistanceBetweenPoints3D(px, py, pz, zone.x, zone.y, zone.z)
        return dist <= zone.radius
    elseif zone.type == "cuboid" then
        return (px >= zone.x and px <= zone.x + zone.width) and (py >= zone.y and py <= zone.y + zone.depth) and (pz >= zone.z and pz <= zone.z + zone.height)
    end
    
    return false
end

function createZoneSphere(x, y, z, radius)
    local id = "zone_" .. tostring(nextZoneID)
    nextZoneID = nextZoneID + 1
    
    zones[id] = {
        type = "sphere",
        x = x, y = y, z = z, 
        radius = radius,
        isPlayerInside = false
    }

    return id
end

function createZoneCuboid(x, y, z, width, depth, height)
    local id = "zone_" .. tostring(nextZoneID)
    nextZoneID = nextZoneID + 1
    
    zones[id] = {
        type = "cuboid",
        x = x, y = y, z = z, 
        width = width, depth = depth, height = height,
        isPlayerInside = false
    }

    return id
end

addEventHandler("onRender", function()
    local cx, cy, cz = getCameraPosition()
    if not cx then return end

    for id, zone in pairs(zones) do
        local isInside = isPointInZone(cx, cy, cz, zone)
        
        if isInside then
            if not zone.isPlayerInside then
                zone.isPlayerInside = true
                triggerEvent("onZoneHit", id, id)
            end
        else
            if zone.isPlayerInside then
                zone.isPlayerInside = false
                triggerEvent("onZoneLeave", id, id)
            end
        end
    end
end)

function clearAllZones()
    zones = {}
    nextZoneID = 1
end

function destroyZone(id)
    if zones[id] then
        zones[id] = nil
        return true
    end

    return false
end
