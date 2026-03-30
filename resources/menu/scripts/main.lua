local menuState = "main"
local sx, sy = getScreenSize()

local btnPlay = {x = 0, y = 0, w = 200, h = 50}
local btnLevel1 = {x = 0, y = 0, w = 0, h = 0}
local btnLevel2 = {x = 0, y = 0, w = 0, h = 0}
local btnLevel3 = {x = 0, y = 0, w = 0, h = 0}
local btnExitImg = {w = 64, h = 64}

local btnResume = {w = 250, h = 40}
local btnSettings = {w = 250, h = 40}
local btnToMenu = {w = 250, h = 40}
local btnQuit = {w = 250, h = 40}

function drawMainMenu()
    if menuState == "hidden" then return end

    sx, sy = getScreenSize()
    
    if menuState == "main" then
        drawImage(0, 0, sx, sy, "assets/textures/menu_background.png")
        drawText("v1.0.0", 5, sy - 22, "assets/fonts/montserrat.ttf", 16, 1, 1, 1, 1, "left")

        btnPlay.x = (sx / 2) - (btnPlay.w / 2)
        btnPlay.y = (sy / 2) - 25
        drawRectangle(btnPlay.x, btnPlay.y, btnPlay.w, btnPlay.h, 0.2, 0.6, 1.0, 1.0)
        drawText("Play", btnPlay.x + (btnPlay.w / 2), btnPlay.y + 10, "assets/fonts/montserrat.ttf", 24, 1, 1, 1, 1, "center")

        btnExitImg.x = sx - 70
        btnExitImg.y = sy - 70
        drawImage(btnExitImg.x, btnExitImg.y, btnExitImg.w, btnExitImg.h, "assets/textures/exit.png", 1, 1, 1, 1)

    elseif menuState == "levels" then
        drawImage(0, 0, sx, sy, "assets/textures/menu_background.png")
        drawText("Choose a Level", sx / 2, 80, "assets/fonts/montserrat.ttf", 40, 1, 1, 1, 1, "center")

        local boxW, boxH = 280, 280
        local gap = 50
        local totalWidth = (boxW * 3) + (gap * 2)
        
        local startX = (sx / 2) - (totalWidth / 2)
        local startY = (sy / 2) - (boxH / 2) + 20

        btnLevel1.x, btnLevel1.y, btnLevel1.w, btnLevel1.h = startX, startY, boxW, boxH
        btnLevel2.x, btnLevel2.y, btnLevel2.w, btnLevel2.h = startX + boxW + gap, startY, boxW, boxH
        btnLevel3.x, btnLevel3.y, btnLevel3.w, btnLevel3.h = startX + (boxW * 2) + (gap * 2), startY, boxW, boxH

        drawRectangle(btnLevel1.x, btnLevel1.y, btnLevel1.w, btnLevel1.h, 0.3, 0.3, 0.3, 1.0)
        drawText("Level 1", btnLevel1.x + (btnLevel1.w / 2), btnLevel1.y + (boxH / 2) - 20, "assets/fonts/montserrat.ttf", 28, 1, 1, 1, 1, "center")
        drawText("Test", btnLevel1.x + (btnLevel1.w / 2), btnLevel1.y + (boxH / 2) + 20, "assets/fonts/montserrat.ttf", 20, 0.8, 0.8, 0.8, 1, "center")

        drawRectangle(btnLevel2.x, btnLevel2.y, btnLevel2.w, btnLevel2.h, 0.4, 0.3, 0.2, 1.0)
        drawText("Level 2", btnLevel2.x + (btnLevel2.w / 2), btnLevel2.y + (boxH / 2) - 20, "assets/fonts/montserrat.ttf", 28, 1, 1, 1, 1, "center")
        drawText("Lava", btnLevel2.x + (btnLevel2.w / 2), btnLevel2.y + (boxH / 2) + 20, "assets/fonts/montserrat.ttf", 20, 0.8, 0.8, 0.8, 1, "center")

        drawRectangle(btnLevel3.x, btnLevel3.y, btnLevel3.w, btnLevel3.h, 0.4, 0.3, 0.2, 1.0)
        drawText("Level 3", btnLevel3.x + (btnLevel3.w / 2), btnLevel3.y + (boxH / 2) - 20, "assets/fonts/montserrat.ttf", 28, 1, 1, 1, 1, "center")
        drawText("Forest", btnLevel3.x + (btnLevel3.w / 2), btnLevel3.y + (boxH / 2) + 20, "assets/fonts/montserrat.ttf", 20, 0.8, 0.8, 0.8, 1, "center")
        
        drawText("< Back", 30, 30, "assets/fonts/montserrat.ttf", 24, 1, 1, 1, 1, "left")

    elseif menuState == "pause" then
        drawRectangle(0, 0, sx, sy, 0.0, 0.0, 0.0, 0.7)
        drawText("Paused", sx / 2, sy/2 - 230, "assets/fonts/montserrat.ttf", 40, 1, 1, 1, 1, "center")

        local startY = sy/2 - 170
        local gap = 60

        btnResume.x, btnResume.y = (sx / 2) - (btnResume.w / 2), startY
        drawRectangle(btnResume.x, btnResume.y, btnResume.w, btnResume.h, 0.2, 0.6, 1.0, 1.0)
        drawText("Resume", sx / 2, btnResume.y + 8, "assets/fonts/montserrat.ttf", 20, 1, 1, 1, 1, "center")

        btnSettings.x, btnSettings.y = (sx / 2) - (btnSettings.w / 2), startY + gap
        drawRectangle(btnSettings.x, btnSettings.y, btnSettings.w, btnSettings.h, 0.4, 0.4, 0.4, 1.0)
        drawText("Settings", sx / 2, btnSettings.y + 8, "assets/fonts/montserrat.ttf", 20, 1, 1, 1, 1, "center")

        btnToMenu.x, btnToMenu.y = (sx / 2) - (btnToMenu.w / 2), startY + (gap * 2)
        drawRectangle(btnToMenu.x, btnToMenu.y, btnToMenu.w, btnToMenu.h, 0.8, 0.5, 0.2, 1.0)
        drawText("Main Menu", sx / 2, btnToMenu.y + 8, "assets/fonts/montserrat.ttf", 20, 1, 1, 1, 1, "center")

        btnQuit.x, btnQuit.y = (sx / 2) - (btnQuit.w / 2), startY + (gap * 3)
        drawRectangle(btnQuit.x, btnQuit.y, btnQuit.w, btnQuit.h, 0.8, 0.2, 0.2, 1.0)
        drawText("Quit Game", sx / 2, btnQuit.y + 8, "assets/fonts/montserrat.ttf", 20, 1, 1, 1, 1, "center")
    end
end
addEventHandler("onRender", drawMainMenu)

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

local function isMouseInBox(mx, my, box)
    return mx >= box.x and mx <= (box.x + box.w) and my >= box.y and my <= (box.y + box.h)
end

function onMenuClick(button, isPressed, mx, my)
    if menuState == "hidden" or not isPressed or button ~= 1 then return end

    if menuState == "main" then
        if isMouseInBox(mx, my, btnPlay) then
            menuState = "levels"
        elseif isMouseInBox(mx, my, btnExitImg) then
            exitGame()
        end

    elseif menuState == "levels" then
        if isMouseInBox(mx, my, btnLevel1) then
            menuState = "hidden"
            showCursor(false) 
            startResource("test")
        elseif isMouseInBox(mx, my, btnLevel2) then
            menuState = "hidden"
            showCursor(false)
            startResource("lava")
        elseif isMouseInBox(mx, my, btnLevel3) then
            menuState = "hidden"
            showCursor(false)
            startResource("forest")
        end
        
        if mx >= 10 and mx <= 150 and my >= 10 and my <= 60 then
            menuState = "main"
        end

    elseif menuState == "pause" then
        if isMouseInBox(mx, my, btnResume) then
            menuState = "hidden"
            showCursor(false)
        elseif isMouseInBox(mx, my, btnSettings) then
            print("Settings opened (Work in progress!)")
        elseif isMouseInBox(mx, my, btnToMenu) then
            stopResource("test")
            stopResource("lava")
            stopResource("forest")
            setCameraPosition(0, 5, 50)
            menuState = "main"
        elseif isMouseInBox(mx, my, btnQuit) then
            exitGame()
        end
    end
end
addEventHandler("onClick", onMenuClick)

showCursor(true)
