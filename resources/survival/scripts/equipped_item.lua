local activeHandItem = nil
local activeItemId = nil

local currentHandRot = { rx = 0, ry = 0, rz = 0 }
local currentHandOffset = { right = 0.4, up = -0.35, forward = 0.55 } 

local isChopping = false
local chopProgress = 0.0
local chopSpeed = 4.0

function getActiveItemId()
    return activeItemId
end

function performChopAnimation()
    if not isChopping and activeHandItem then
        isChopping = true
        chopProgress = 0.0
    end
end

function equipItem(itemId)
    if activeHandItem then
        destroyElement(activeHandItem)
        activeHandItem = nil
        activeItemId = nil
    end

    if not itemId or not ITEMS[itemId] then 
        return
    end
    local itemData = ITEMS[itemId]
    
    if not itemData.model then
        return
    end

    currentHandRot = itemData.handRotation or { rx = 0, ry = 0, rz = 0 }
    currentHandOffset = itemData.handOffset or { right = 0.4, up = -0.35, forward = 0.55 }
    local s = itemData.handScale or 1.0

    activeHandItem = createObject(itemData.model, itemData.texture, 0, 0, 0, 0, 0, 0, s, s, s)
    
    if activeHandItem then
        setElementCollisionsEnabled(activeHandItem, false)
        activeItemId = itemId
    end
end

addEventHandler("onRender", function(dt)
    if not activeHandItem then
        return
    end
    
    dt = dt or 0.016

    local chopRotOffset = 0
    local chopPosOffset = 0
    
    if isChopping then
        chopProgress = chopProgress + (dt * chopSpeed)
        if chopProgress >= 1.0 then
            chopProgress = 0.0
            isChopping = false
        else
            local swing = math.sin(chopProgress * math.pi)
            chopRotOffset = swing * 60.0
            chopPosOffset = swing * 0.15
        end
    end

    local finalRight = currentHandOffset.right
    local finalUp = currentHandOffset.up - (chopPosOffset * 0.5)
    local finalForward = currentHandOffset.forward + chopPosOffset
    
    local finalRx = currentHandRot.rx - chopRotOffset
    local finalRy = currentHandRot.ry
    local finalRz = currentHandRot.rz 

    attachObjectToCamera(activeHandItem, finalRight, finalUp, -finalForward, finalRx, finalRy, finalRz)
end)
