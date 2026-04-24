local currentTarget = nil 
local interactDistance = 3.0 

local isPlacing = false
local placingItemId = nil
local ghostObject = nil
local justStartedPlacing = false

local placedCraftingStations = {}

local lastPx, lastPy, lastPz = nil, nil, nil
local walkSound = nil

addEventHandler("onRender", function(dt)
    local px, py, pz = getCameraPosition()
    local dx, dy, dz = getCameraForward()

    if isPlacing and ghostObject then
        local placeDistance = 3.5
        local tx = px + dx * placeDistance
        local ty = py + dy * placeDistance
        
        local tz = getTerrainHeight(tx, ty)
        
        local itemData = ITEMS[placingItemId]
        if itemData and itemData.placeOffsetZ then
            tz = tz + itemData.placeOffsetZ
        end
        
        setElementPosition(ghostObject, tx, ty, tz)
        return 
    end

    local tx = px + dx * interactDistance
    local ty = py + dy * interactDistance
    local tz = pz + dz * interactDistance
    
    local hit, hX, hY, hZ, hitId = processLineOfSight(px, py, pz, tx, ty, tz)
    
    if hit and hitId ~= -1 then
        local itemId = getElementData(hitId, "item:id")
        if itemId then
            currentTarget = hitId
        else
            currentTarget = nil
        end
    else
        currentTarget = nil
    end
end)

addEventHandler("onRender", function(dt)
    if isDead then return end
    dt = dt or 0.016

    local px, py, pz = getCameraPosition()
    if px then
        local isMoving = false
        if lastPx then
            local dist = math.sqrt((px-lastPx)^2 + (py-lastPy)^2 + (pz-lastPz)^2)
            local speed = dist / dt
            
            if speed > 1.0 then 
                isMoving = true 
            end
        end
        lastPx, lastPy, lastPz = px, py, pz

        if isMoving then
            if not walkSound then
                walkSound = playSound("assets/sounds/walk.mp3", true)
                if walkSound and setSoundVolume then
                    setSoundVolume(walkSound, 0.1) 
                end
            end
        else
            if walkSound then
                stopSound(walkSound)
                walkSound = nil
            end
        end
    end
end)

addEventHandler("onPostRender", function()
    local sw, sh = getScreenSize()
    
    if isPlacing and not isInvOpen then
        local itemName = ITEMS[placingItemId] and ITEMS[placingItemId].name or "Tárgy"
        
        drawText(itemName .. " preview", sw/2, sh/2 + 40, "/assets/fonts/montserrat.ttf", 20, 1, 1, 1, 1, "center")
        drawText("[Left click] - Place   |   [Right click] - Cancel", sw/2, sh/2 + 70, "/assets/fonts/montserrat.ttf", 16, 1, 0.8, 0.2, 1, "center")
        return
    end

    if currentTarget and not isInvOpen then
        local name = getElementData(currentTarget, "item:name") or "Item"
        local itemId = getElementData(currentTarget, "item:id")
        
        drawText(name, sw/2, sh/2 + 40, "/assets/fonts/montserrat.ttf", 20, 1, 1, 1, 1, "center")
        
        if itemId == "tree" or itemId == "rock_node" then
            local hpKey = (itemId == "tree") and "tree:hp" or "rock:hp"
            local hp = getElementData(currentTarget, hpKey) or 100
            drawText("HP: " .. math.floor(hp) .. "/100", sw/2, sh/2 + 70, "/assets/fonts/montserrat.ttf", 16, 0.2, 0.8, 0.2, 1, "center")
        else
            drawText("[E] - Pick up", sw/2, sh/2 + 70, "/assets/fonts/montserrat.ttf", 16, 1, 0.8, 0.2, 1, "center")
        end
    end
end)

addEventHandler("onKey", function(key, state)
    if isDead then return end

    if key == "E" and state == true and currentTarget then
        local itemId = getElementData(currentTarget, "item:id")
        
        if itemId and itemId ~= "tree" and itemId ~= "rock_node" then
            local linkedFireId = getElementData(currentTarget, "fire:id")
            if linkedFireId and destroyFire then
                destroyFire(linkedFireId)
            end

            local linkedSound = getElementData(currentTarget, "fire:sound")
            if linkedSound then
                stopSound(linkedSound)
            end

            if itemId == "campfire" then
                for i = #placedCraftingStations, 1, -1 do
                    if placedCraftingStations[i] == currentTarget then
                        table.remove(placedCraftingStations, i)
                        break
                    end
                end
            end

            if giveItem(itemId, 1) then
                destroyElement(currentTarget)
                currentTarget = nil
            end
        end
    end
end)

addEventHandler("onClick", function(button, isPressed)
    if isDead then return end
    
    if isCursorVisible() then
        return
    end
    
    if button == 1 and isPressed == true then
        
        if justStartedPlacing then
            justStartedPlacing = false
            return
        end
        
        if isPlacing and ghostObject and placingItemId then
            if takeItem(placingItemId, 1) then
                setElementCollisionsEnabled(ghostObject, true)
                setElementData(ghostObject, "item:id", placingItemId)
                setElementData(ghostObject, "item:name", ITEMS[placingItemId].name)
                
                if placingItemId == "campfire" then
                    local cx, cy, cz = getElementPosition(ghostObject)
                    local fireId = createFire(cx, cy, cz + 0.3, 0.8, 0.8)
                    if fireId then
                        setElementData(ghostObject, "fire:id", fireId)
                    end
                    
                    local fireSound = playSound3D("/assets/sounds/fire.wav", cx, cy, cz + 0.3, true)
                    if fireSound then
                        if setSoundVolume then
                            setSoundVolume(fireSound, 0.5)
                        end
                        
                        setElementData(ghostObject, "fire:sound", fireSound)
                    end

                    table.insert(placedCraftingStations, ghostObject)
                end
                
                isPlacing = false
                ghostObject = nil
                placingItemId = nil
            else
                cancelPlacement()
            end

            return
        end

        local heldItem = getActiveItemId and getActiveItemId() or nil
        if heldItem and ITEMS[heldItem] and ITEMS[heldItem].type == "tool" then
            
            if performChopAnimation then
                performChopAnimation()
            end
            
            if currentTarget then
                local targetId = getElementData(currentTarget, "item:id")
                
                if targetId == "tree" and heldItem == "stone_axe" then
                    local sound = playSound("assets/sounds/axe_hit.mp3")
                    if sound and setSoundVolume then
                        setSoundVolume(sound, 0.5)
                    end
                    local hp = (getElementData(currentTarget, "tree:hp") or 100) - 25
                    if hp <= 0 then
                        fallTree(currentTarget) 
                        local soundFall = playSound("assets/sounds/tree_fall.mp3")
                        if soundFall and setSoundVolume then setSoundVolume(soundFall, 0.5) end
                    else
                        setElementData(currentTarget, "tree:hp", hp)
                    end
                elseif targetId == "rock_node" and heldItem == "pickaxe" then
                    local sound = playSound("assets/sounds/pickaxe_hit.mp3")
                    if sound and setSoundVolume then
                        setSoundVolume(sound, 0.5)
                    end
                    local hp = (getElementData(currentTarget, "rock:hp") or 100) - 20
                    if hp <= 0 then
                        breakRock(currentTarget)
                    else
                        setElementData(currentTarget, "rock:hp", hp)
                    end
                end
            end
        end

    elseif (button == 3 or button == 2) and isPressed == true then
        if isPlacing then
            cancelPlacement()
        end
    end
end)

function startPlacementMode(itemId)
    if isPlacing then
        cancelPlacement()
    end
    
    local itemData = ITEMS[itemId]
    if not itemData or not itemData.model then
        return
    end
    
    isPlacing = true
    justStartedPlacing = true
    placingItemId = itemId
    
    local s = itemData.placeScale or 1.0
    local rx = itemData.placeRotation and itemData.placeRotation.rx or 0
    local ry = itemData.placeRotation and itemData.placeRotation.ry or 0
    local rz = itemData.placeRotation and itemData.placeRotation.rz or 0
    
    ghostObject = createObject(itemData.model, itemData.texture, 0, 0, 0, rx, ry, rz, s, s, s)
    
    if ghostObject then
        setElementCollisionsEnabled(ghostObject, false)
    end
end

function cancelPlacement()
    if ghostObject then
        destroyElement(ghostObject)
        ghostObject = nil
    end
    isPlacing = false
    placingItemId = nil
end

function isNearItem(targetItemId, maxDist)
    local px, py, pz = getCameraPosition() 
    
    for _, objId in ipairs(placedCraftingStations) do
        if getElementData(objId, "item:id") == targetItemId then
            local ox, oy, oz = getElementPosition(objId)
            local dist = math.sqrt((px-ox)^2 + (py-oy)^2 + (pz-oz)^2)
            
            if dist <= maxDist then
                return true
            end
        end
    end

    return false
end

addEventHandler("onRender", function(dt)
    if isDead then return end
    dt = dt or 0.016
    
    local px, py, pz = getCameraPosition()
    if not px then return end

    for _, stationId in ipairs(placedCraftingStations) do
        if getElementData(stationId, "item:id") == "campfire" and getElementData(stationId, "fire:id") then
            
            local ox, oy, oz = getElementPosition(stationId)
            
            local distXY = math.sqrt((px-ox)^2 + (py-oy)^2)
            local distZ = math.abs(pz - oz)
            
            if distXY < 1.2 and distZ < 2.5 then
                
                if modifyStat then
                    modifyStat("health", -20 * dt)
                end
            end
        end
    end
end)