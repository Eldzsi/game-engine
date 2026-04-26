local sx, sy = getScreenSize()

local font = "/assets/fonts/montserrat.ttf"

local menuState = "main"

local levels = {"test", "desert", "lava", "survival"}
local levelButtons = {} 

local btnPlay = {x = 0, y = 0, w = 200, h = 50}
local btnExitImg = {x = 0, y = 0, w = 64, h = 64}
local btnResume = {x = 0, y = 0, w = 250, h = 40}
local btnToMenu = {x = 0, y = 0, w = 250, h = 40}
local btnQuit = {x = 0, y = 0, w = 250, h = 40}

local function isMouseInBox(mx, my, box)
    return mx >= box.x and mx <= (box.x + box.w) and my >= box.y and my <= (box.y + box.h)
end

function drawMenu()
    if menuState == "hidden" then return end

    sx, sy = getScreenSize()
    
    if menuState == "main" then
        drawMainMenu()
    elseif menuState == "levels" then
        drawLevels()
    elseif menuState == "pause" then
        drawPauseMenu()
    end
end
addEventHandler("onRender", drawMenu)

function drawMainMenu()
    drawImage(0, 0, sx, sy, "assets/textures/menu_background.jpg")
    drawText("v1.0.0", 5, sy - 22, font, 16, 1, 1, 1, 1, "left")

    btnPlay.x = (sx / 2) - (btnPlay.w / 2)
    btnPlay.y = (sy / 2) - 25
    drawRectangle(btnPlay.x, btnPlay.y, btnPlay.w, btnPlay.h, 0.2, 0.6, 1.0, 1.0)
    drawText("Play", btnPlay.x + (btnPlay.w / 2), btnPlay.y + 10, font, 24, 1, 1, 1, 1, "center")

    btnExitImg.x = sx - 70
    btnExitImg.y = sy - 70
    drawImage(btnExitImg.x, btnExitImg.y, btnExitImg.w, btnExitImg.h, "assets/textures/exit.png", 1, 1, 1, 1)
end

function drawLevels()
    drawImage(0, 0, sx, sy, "assets/textures/menu_background.jpg")

    local boxW, boxH = 220, 220
    local gapX, gapY = 30, 40
    local cols = 4

    levelButtons = {} 
    
    local validLevels = {}
    for _, resName in ipairs(levels) do
        if resName ~= "" and #validLevels < 8 then
            table.insert(validLevels, resName)
        end
    end

    local totalValid = #validLevels
    local totalGridHeight = 0
    local startY = sy / 2

    if totalValid > 0 then
        local totalRows = math.ceil(totalValid / cols)
        totalGridHeight = (totalRows * boxH) + ((totalRows - 1) * gapY)
        startY = (sy / 2) - (totalGridHeight / 2)
    end

    drawText("Choose a Level", sx / 2, startY - 75, font, 40, 1, 1, 1, 1, "center")

    if totalValid > 0 then
        for i, resName in ipairs(validLevels) do
            local index = i - 1
            local col = index % cols
            local row = math.floor(index / cols)

            local itemsInThisRow = cols
            if row == math.floor((totalValid - 1) / cols) then
                itemsInThisRow = totalValid - (row * cols)
            end

            local rowWidth = (itemsInThisRow * boxW) + ((itemsInThisRow - 1) * gapX)
            local rowStartX = (sx / 2) - (rowWidth / 2)

            local x = rowStartX + (col * (boxW + gapX))
            local y = startY + (row * (boxH + gapY))

            local info = (type(getResourceInfo) == "function") and getResourceInfo(resName) or nil
            local displayName = (info and info.name) or resName

            local border = 5
            local titleHeight = 45

            drawRectangle(x, y, boxW, boxH, 0.12, 0.12, 0.12, 1.0)

            if info and info.thumbnail then
                local imgW = boxW - (border * 2)
                local imgH = boxH - titleHeight - border
                
                drawImage(x + border, y + border, imgW, imgH, info.thumbnail)
                                
                drawText(displayName, x + (boxW / 2), y + boxH - (titleHeight / 2) - 10, font, 18, 1, 1, 1, 1, "center")
            else
                local innerW = boxW - (border * 2)
                local innerH = boxH - (border * 2)
                drawRectangle(x + border, y + border, innerW, innerH, 0.25, 0.3, 0.35, 1.0)
                drawText(displayName, x + (boxW / 2), y + (boxH / 2) - 10, font, 24, 1, 1, 1, 1, "center")
            end

            table.insert(levelButtons, {x = x, y = y, w = boxW, h = boxH, resource = resName})
        end
    end
    
    drawText("< Back", 30, 30, font, 24, 1, 1, 1, 1, "left")
end

function drawPauseMenu()
    drawRectangle(0, 0, sx, sy, 0.0, 0.0, 0.0, 0.7)
    drawText("Paused", sx / 2, sy/2 - 145, font, 40, 1, 1, 1, 1, "center")

    local startY = sy/2 - 85
    local gap = 60

    btnResume.x, btnResume.y = (sx / 2) - (btnResume.w / 2), startY
    drawRectangle(btnResume.x, btnResume.y, btnResume.w, btnResume.h, 0.2, 0.6, 1.0, 1.0)
    drawText("Resume", sx / 2, btnResume.y + 8, font, 20, 1, 1, 1, 1, "center")

    btnToMenu.x, btnToMenu.y = (sx / 2) - (btnToMenu.w / 2), startY + gap
    drawRectangle(btnToMenu.x, btnToMenu.y, btnToMenu.w, btnToMenu.h, 0.8, 0.5, 0.2, 1.0)
    drawText("Main Menu", sx / 2, btnToMenu.y + 8, font, 20, 1, 1, 1, 1, "center")

    btnQuit.x, btnQuit.y = (sx / 2) - (btnQuit.w / 2), startY + (gap * 2)
    drawRectangle(btnQuit.x, btnQuit.y, btnQuit.w, btnQuit.h, 0.8, 0.2, 0.2, 1.0)
    drawText("Quit Game", sx / 2, btnQuit.y + 8, font, 20, 1, 1, 1, 1, "center")

    drawResourceInfo()
end

function drawResourceInfo(currentRes)
    local currentRes = (type(getRunningGamemode) == "function") and getRunningGamemode() or nil
    if currentRes then
        local info = (type(getResourceInfo) == "function") and getResourceInfo(currentRes) or nil
        if info then
            local pW, pH = 300, 120
            local px, py = sx - pW - 20, sy - pH - 20
            drawRectangle(px, py, pW, pH, 0.15, 0.15, 0.15, 0.9)
            drawText(info.name, px + 10, py + 6, font, 20, 1, 0.8, 0, 1, "left")
            drawText("Author: " .. (info.author or "Unknown"), px + 10, py + 34, font, 14, 0.8, 0.8, 0.8, 1, "left")
            drawText("Version: " .. (info.version or "1.0"), px + 10, py + 54, font, 14, 0.8, 0.8, 0.8, 1, "left")
            drawText("Description: " .. info.description or "", px + 10, py + 75, font, 12, 0.6, 0.6, 0.6, 1, "left")
        end
    end
end

function onMenuKey(key, isPressed)
    if isPressed and key == "Escape" then
        if menuState == "hidden" then
            menuState = "pause"
            showCursor(true)
        elseif menuState == "pause" then
            menuState = "hidden"
            showCursor(false)
        end
    end
end
addEventHandler("onKey", onMenuKey)

function onMenuClick(button, isPressed, mx, my)
    if menuState == "hidden" or not isPressed or button ~= 1 then return end

    if menuState == "main" then
        if isMouseInBox(mx, my, btnPlay) then
            menuState = "levels"
        elseif isMouseInBox(mx, my, btnExitImg) then
            exitGame()
        end

    elseif menuState == "levels" then
        for i, btn in ipairs(levelButtons) do
            if isMouseInBox(mx, my, btn) then
                menuState = "hidden"
                showCursor(false) 
                startResource(btn.resource)
                return
            end
        end
        
        if mx >= 10 and mx <= 150 and my >= 10 and my <= 60 then
            menuState = "main"
        end

    elseif menuState == "pause" then
        if isMouseInBox(mx, my, btnResume) then
            menuState = "hidden"
            showCursor(false)
        elseif isMouseInBox(mx, my, btnToMenu) then
            for _, resName in ipairs(levels) do
                if resName ~= "" then
                    stopResource(resName)
                end
            end
            setCameraPosition(0, 5, 50)
            menuState = "main"
        elseif isMouseInBox(mx, my, btnQuit) then
            exitGame()
        end
    end
end
addEventHandler("onClick", onMenuClick)

addEventHandler("onGamemodeStart", function(resName)
    menuState = "hidden"
    showCursor(false)
end)

addEventHandler("onGamemodeStop", function(resName)
    menuState = "main"
    showCursor(true)
end)

showCursor(true)
