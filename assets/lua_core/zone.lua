local colShapes = {}
local nextColShapeID = 1

function createZoneSphere(x, y, z, radius)
    local id = "zone_" .. tostring(nextColShapeID)
    nextColShapeID = nextColShapeID + 1
    
    colShapes[id] = {
        x = x, y = y, z = z, radius = radius,
        isPlayerInside = false
    }
    return id
end

local function getDistanceBetweenPoints3D(x1, y1, z1, x2, y2, z2)
    local dx = x1 - x2; local dy = y1 - y2; local dz = z1 - z2
    return math.sqrt(dx*dx + dy*dy + dz*dz)
end

addEventHandler("onRender", function()
    local cx, cy, cz = getCameraPosition()
    if not cx then return end

    for id, shape in pairs(colShapes) do
        local dist = getDistanceBetweenPoints3D(cx, cy, cz, shape.x, shape.y, shape.z)
        if dist <= shape.radius then
            if not shape.isPlayerInside then
                shape.isPlayerInside = true
                triggerEvent("onColShapeHit", id, id)
            end
        else
            if shape.isPlayerInside then
                shape.isPlayerInside = false
                triggerEvent("onColShapeLeave", id, id)
            end
        end
    end
end)

function clearAllZones()
    colShapes = {}
    nextColShapeID = 1
end

function destroyZone(id)
    if colShapes[id] then
        colShapes[id] = nil
        return true
    end
    return false
end
