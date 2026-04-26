local spawnPos = {0, 0, 4}
local lavaLevel = -0.4
local checkpointRadius = 2
local checkpoints = {
    {x = 0, y = 51, z = 12},
}

local gameState = {
    coins = 0,
    timeElapsed = 0,
    isWon = false,
    lives = 3,
    lastSpawn = {spawnPos[1], spawnPos[2], spawnPos[3]},
    activeCheckpoint = 0,
    checkpointMessageTimer = 0
}

local function respawnPlayer()
    setCameraPosition(gameState.lastSpawn[1], gameState.lastSpawn[2], gameState.lastSpawn[3])
    setCameraRotation(0, 0, -90)
end

local function resetGame()
    gameState.coins = 0
    gameState.timeElapsed = 0
    gameState.isWon = false
    gameState.lives = 3
    gameState.activeCheckpoint = 0
    gameState.checkpointMessageTimer = 0
    gameState.lastSpawn = {spawnPos[1], spawnPos[2], spawnPos[3]}
    
    reloadMap()
    
    setCameraPosition(gameState.lastSpawn[1], gameState.lastSpawn[2], gameState.lastSpawn[3])
    setCameraRotation(0, 0, -90)

    setAmbientLight(0.4, 0.4, 0.4)
    setSkyColor(0.05, 0.05, 0.5)
    setDirectionalLight(-0.5, -1.0, -0.5, 1.0, 0.4, 0.4)
end

local function checkCheckpoints(px, py, pz)
    for i, cp in ipairs(checkpoints) do
        if i > gameState.activeCheckpoint then
            local dist = math.sqrt((px - cp.x)^2 + (py - cp.y)^2 + (pz - cp.z)^2)
            if dist < checkpointRadius then
                gameState.activeCheckpoint = i
                gameState.lastSpawn = {cp.x, cp.y, cp.z + 1}
                gameState.checkpointMessageTimer = 3
            end
        end
    end
end

addEventHandler("onRender", function(dt)
    local sx, sy = getScreenSize()

    local startX = 30
    local startY = 30
    local iconSize = 32
    
    drawImage(startX, startY, iconSize, iconSize, "assets/icons/heart.png")
    drawText("x" .. gameState.lives, startX + iconSize + 8, startY, "/assets/fonts/montserrat.ttf", 26, 1, 1, 1, 1, "left")
    
    startY = startY + 40
    drawImage(startX, startY, iconSize, iconSize, "assets/icons/coin.png")
    drawText("x" .. gameState.coins, startX + iconSize + 8, startY, "/assets/fonts/montserrat.ttf", 26, 1, 1, 1, 1, "left")

    startY = startY + 40
    drawImage(startX, startY, iconSize, iconSize, "assets/icons/checkpoint.png")
    local cpText = gameState.activeCheckpoint .. " / " .. #checkpoints + 1
    drawText(cpText, startX + iconSize + 8, startY, "/assets/fonts/montserrat.ttf", 26, 1, 1, 1, 1, "left")

    if not gameState.isWon then
        gameState.timeElapsed = gameState.timeElapsed + dt
        drawText(string.format("TIME: %.1f", gameState.timeElapsed), sx - 30, 30, "/assets/fonts/montserrat.ttf", 26, 1, 1, 1, 1, "right")

        local px, py, pz = getCameraPosition()
        if not px then
            return
        end
        
        checkCheckpoints(px, py, pz)

        if (pz - 1.8) < lavaLevel then
            gameState.lives = gameState.lives - 1
            if gameState.lives > 0 then
                respawnPlayer()
            else
                resetGame()
            end

            return
        end
    else
        drawRectangle(0, 0, sx, sy, 0, 0, 0, 0.7)
        drawText("VICTORY!", sx/2, sy/2 - 100, "/assets/fonts/montserrat.ttf", 64, 0.2, 1, 0.2, 1, "center")
        drawText("Final Time: " .. string.format("%.1f", gameState.timeElapsed) .. "s", sx/2, sy/2 - 30, "/assets/fonts/montserrat.ttf", 32, 1, 1, 1, 1, "center")
        drawText("Coins Collected: " .. gameState.coins, sx/2, sy/2 + 10, "/assets/fonts/montserrat.ttf", 32, 1, 0.8, 0.2, 1, "center")
    end
end)

addEventHandler("onPickupHit", function(pickupType)
    destroyElement(source)
    if pickupType == "coin" then
        gameState.coins = gameState.coins + 1    
    elseif pickupType == "finish" then
        gameState.isWon = true
        gameState.activeCheckpoint = #checkpoints + 1
    end
end)

addEventHandler("onKey", function(key, state)
    if gameState.isWon and state == true then
        resetGame()
    end
end)

addEventHandler("onClick", function(button, isPressed)
    if gameState.isWon and isPressed == true then
        resetGame()
    end
end)

resetGame()
