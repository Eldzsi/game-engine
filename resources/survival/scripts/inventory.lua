local playerInventory = {}

local isInvOpen = false
local currentTab = "inventory"

function hasItem(itemId, amount)
    amount = amount or 1
    local currentCount = playerInventory[itemId] or 0

    return currentCount >= amount
end

function giveItem(itemId, amount)
    amount = amount or 1
    if not ITEMS[itemId] then
        return false
    end
    
    playerInventory[itemId] = (playerInventory[itemId] or 0) + amount

    return true
end

function takeItem(itemId, amount)
    amount = amount or 1
    if not hasItem(itemId, amount) then
        return false
    end
    
    playerInventory[itemId] = playerInventory[itemId] - amount
    if playerInventory[itemId] <= 0 then
        playerInventory[itemId] = nil
    end

    return true
end

local function isMouseInBox(mx, my, x, y, w, h)
    return mx >= x and mx <= (x + w) and my >= y and my <= (y + h)
end

local function wordWrap(text, maxLineLength)
    local lines = {}
    local currentLine = ""
    
    for word in string.gmatch(text, "%S+") do
        if string.len(currentLine) + string.len(word) > maxLineLength then
            table.insert(lines, currentLine)
            currentLine = word .. " "
        else
            currentLine = currentLine .. word .. " "
        end
    end
    table.insert(lines, currentLine)
    
    return lines
end

local function drawTooltip(mx, my, itemId)
    local itemData = ITEMS[itemId]
    if not itemData then
        return
    end
    
    local name = itemData.name
    local desc = itemData.description or ""
    local actionText = ""
    
    local currentHeld = getActiveItemId and getActiveItemId() or nil
    
    if currentTab == "inventory" then
        if itemData.type == "food" then 
            actionText = "[Left click] - Eat"
        elseif itemData.type == "tool" then 
            if currentHeld == itemId then
                actionText = "[Left click] - Unequip"
            else
                actionText = "[Left click] - Equip"
            end
        elseif itemData.type == "placeable" then 
            actionText = "[Left click] - Place"
        elseif itemData.type == "resource" then 
            actionText = "Resource for crafting"
        else 
            actionText = "" 
        end
    end
    
    local descLines = wordWrap(desc, 38)
    local boxW = 300
    
    local actionPadding = (actionText == "") and 0 or 25
    local boxH = 45 + (#descLines * 16) + actionPadding 
    
    local tx = mx + 15
    local ty = my + 15
    
    drawRectangle(tx, ty, boxW, boxH, 0.1, 0.1, 0.1, 0.95)
    drawRectangle(tx, ty, boxW, 2, 0.8, 0.6, 0.2, 1)
    
    drawText(name, tx + 10, ty + 10, "/assets/fonts/montserrat.ttf", 16, 1, 0.8, 0.2, 1, "left")
    
    local currentY = ty + 35
    for i, line in ipairs(descLines) do
        drawText(line, tx + 10, currentY, "/assets/fonts/montserrat.ttf", 12, 0.7, 0.7, 0.7, 1, "left")
        currentY = currentY + 16
    end
    
    if actionText ~= "" then
        drawText(actionText, tx + 10, currentY + 8, "/assets/fonts/montserrat.ttf", 13, 0.4, 0.8, 0.4, 1, "left")
    end
end

addEventHandler("onKey", function(key, state)
    if isDead then
        return
    end
    
    if (key == "Tab" or key == "I" or key == "i") and state == true then
        isInvOpen = not isInvOpen
        
        showCursor(isInvOpen, "inventory")
        
        toggleControl("forward", not isInvOpen)
        toggleControl("backward", not isInvOpen)
        toggleControl("left", not isInvOpen)
        toggleControl("right", not isInvOpen)
        toggleControl("jump", not isInvOpen)
        toggleControl("sprint", not isInvOpen)
    end
end)

addEventHandler("onPostRender", function()
    if not isInvOpen then
        return
    end

    local mx, my = getCursorPosition() 
    if not mx then
        mx, my = 0, 0
    end
    
    local sw, sh = getScreenSize()
    local winW, winH = 600, 450
    local winX, winY = (sw - winW) / 2, (sh - winH) / 2
    
    drawRectangle(winX, winY, winW, winH, 0.1, 0.1, 0.1, 0.95)
    
    local tabW = winW / 2
    local invTabColor = currentTab == "inventory" and {1, 0.8, 0.2} or {0.15, 0.15, 0.15}
    local craftTabColor = currentTab == "crafting" and {1, 0.8, 0.2} or {0.15, 0.15, 0.15}
    
    drawRectangle(winX, winY, tabW, 40, invTabColor[1], invTabColor[2], invTabColor[3], 1)
    if currentTab == "inventory" then
        drawText("INVENTORY", winX + tabW/2, winY + 8, "/assets/fonts/montserrat.ttf", 20, 0, 0, 0, 1, "center")
    else
        drawText("INVENTORY", winX + tabW/2, winY + 8, "/assets/fonts/montserrat.ttf", 20, 1, 1, 1, 1, "center")
    end
    
    drawRectangle(winX + tabW, winY, tabW, 40, craftTabColor[1], craftTabColor[2], craftTabColor[3], 1)
    if currentTab == "crafting" then
        drawText("CRAFTING", winX + tabW + tabW/2, winY + 8, "/assets/fonts/montserrat.ttf", 20, 0, 0, 0, 1, "center")

    else
        drawText("CRAFTING", winX + tabW + tabW/2, winY + 8, "/assets/fonts/montserrat.ttf", 20, 1, 1, 1, 1, "center")
    end
    
    local contentY = winY + 60
    local hoveredItem = nil
    
    if currentTab == "inventory" then
        local itemY = contentY
        
        local currentHeld = getActiveItemId and getActiveItemId() or nil
        
        for id, count in pairs(playerInventory) do
            local itemData = ITEMS[id]
            local itemName = itemData and itemData.name or id
            
            drawRectangle(winX + 20, itemY, winW - 40, 50, 0.15, 0.15, 0.15, 0.8)
            
            if isMouseInBox(mx, my, winX + 20, itemY, winW - 40, 50) then
                hoveredItem = id
                drawRectangle(winX + 20, itemY, winW - 40, 50, 1.0, 1.0, 1.0, 0.1)
            end
            
            if itemData.icon then
                drawImage(winX + 30, itemY + 8, 32, 32, itemData.icon)
            else
                drawRectangle(winX + 25, itemY + 1, 48, 48, 0.3, 0.3, 0.3, 1.0)
            end
            
            local r, g, b = 1, 0.8, 0.2
            local equippedText = ""
            
            if currentHeld == id then
                r, g, b = 0.3, 0.9, 0.3
                equippedText = " (Equipped)"
            end
            
            drawText(itemName .. equippedText .. " (x" .. count .. ")", winX + 75, itemY + 15, "/assets/fonts/montserrat.ttf", 16, r, g, b, 1, "left")
            
            itemY = itemY + 55
        end
    elseif currentTab == "crafting" then
        local craftY = contentY
        for recipeId, recipe in pairs(RECIPES) do
            local itemData = ITEMS[recipeId]
            local itemName = ITEMS[recipeId] and ITEMS[recipeId].name or recipeId
            
            drawRectangle(winX + 20, craftY, winW - 40, 50, 0.15, 0.15, 0.15, 0.8)
            
            if isMouseInBox(mx, my, winX + 20, craftY, winW - 40, 50) then
                hoveredItem = recipeId
                drawRectangle(winX + 20, craftY, winW - 40, 50, 1.0, 1.0, 1.0, 0.1)
            end
            
            if itemData and itemData.icon then
                drawImage(winX + 30, craftY + 8, 32, 32, itemData.icon)
            else
                drawRectangle(winX + 30, craftY + 10, 30, 30, 0.2, 0.2, 0.2, 1.0)
            end

            drawText(itemName, winX + 75, craftY + 6, "/assets/fonts/montserrat.ttf", 16, 1, 0.8, 0.2, 1, "left")
            
            local reqText = ""
            local canCraft = true
            for _, req in ipairs(recipe.requires) do
                local reqName = ITEMS[req.item] and ITEMS[req.item].name or req.item
                local hasCount = playerInventory[req.item] or 0
                
                if hasCount < req.count then canCraft = false end
                reqText = reqText .. reqName .. ": " .. hasCount .. "/" .. req.count .. "   "
            end
            drawText(reqText, winX + 75, craftY + 27, "/assets/fonts/montserrat.ttf", 13, 0.8, 0.8, 0.8, 1, "left")
            
            local btnColor = canCraft and {0.2, 0.6, 0.2} or {0.5, 0.2, 0.2}
            drawRectangle(winX + winW - 130, craftY + 10, 100, 30, btnColor[1], btnColor[2], btnColor[3], 1)
            drawText("Craft", winX + winW - 80, craftY + 15, "/assets/fonts/montserrat.ttf", 16, 1, 1, 1, 1, "center")
            
            craftY = craftY + 55
        end
    end
    
    if hoveredItem then
        drawTooltip(mx, my, hoveredItem)
    end
end)

addEventHandler("onClick", function(button, isPressed, mx, my)
    if not isInvOpen or button ~= 1 or not isPressed then
        return
    end
    
    local sw, sh = getScreenSize()
    local winW, winH = 600, 450
    local winX, winY = (sw - winW) / 2, (sh - winH) / 2
    local tabW = winW / 2
    
    if isMouseInBox(mx, my, winX, winY, tabW, 40) then
        currentTab = "inventory"
    elseif isMouseInBox(mx, my, winX + tabW, winY, tabW, 40) then
        currentTab = "crafting"
    end

    if currentTab == "inventory" then
        local itemY = winY + 60
        for id, count in pairs(playerInventory) do
            if count > 0 then
                if isMouseInBox(mx, my, winX + 20, itemY, winW - 40, 50) then
                    if equipItem then
                        local currentHeld = getActiveItemId and getActiveItemId() or nil
                        
                        if currentHeld == id then
                            equipItem(nil)
                        else
                            if ITEMS[id].type == "tool" or ITEMS[id].type == "weapon" then
                                equipItem(id)
                            
                            elseif ITEMS[id].type == "food" then
                                if takeItem(id, 1) then
                                    local nutrition = ITEMS[id].nutrition or 10
                                    
                                    if modifyStat then
                                        modifyStat("hunger", nutrition)
                                        modifyStat("health", 5)
                                        playSound("assets/sounds/eating.mp3")
                                    end
                                end
                                
                            elseif ITEMS[id].type == "placeable" then
                                if startPlacementMode then
                                    startPlacementMode(id)
                                    
                                    isInvOpen = false
                                    showCursor(false)
                                    toggleControl("forward", true)
                                    toggleControl("backward", true)
                                    toggleControl("left", true)
                                    toggleControl("right", true)
                                    toggleControl("jump", true)
                                    toggleControl("sprint", true)
                                end
                            end
                        end
                    end
                end
            end
            itemY = itemY + 55 
        end
    end
    
    if currentTab == "crafting" then
        local craftY = winY + 60
        for recipeId, recipe in pairs(RECIPES) do
            if isMouseInBox(mx, my, winX + winW - 130, craftY + 10, 100, 30) then
                if craftItem then
                    craftItem(recipeId)
                end
            end
            craftY = craftY + 55
        end
    end
end)

function craftItem(recipeId)
    local recipe = RECIPES[recipeId]
    if not recipe then
        return false
    end
    
    if recipe.nearObject then
        if not isNearItem(recipe.nearObject, recipe.nearDistance or 3.0) then
            return false
        end
    end

    for _, req in ipairs(recipe.requires) do
        if not hasItem(req.item, req.count) then
            local missingItemName = ITEMS[req.item].name
            return false
        end
    end
    
    for _, req in ipairs(recipe.requires) do
        takeItem(req.item, req.count)
    end
    
    local amountToGive = recipe.yields or 1
    giveItem(recipeId, amountToGive)
    
    return true
end

giveItem("stone_axe", 1)
giveItem("pickaxe", 1)
giveItem("campfire", 1)
