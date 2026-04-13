local CONFIG = {
    totalCoins = 5,
    timeLimit = 90.0,
    spawnPos = {0, 0, 4},
    lavaLevel = -0.4
}

local gameState = {
    coins = 0,
    timeLeft = CONFIG.timeLimit,
    isWon = false,
    spawnImmunity = 0.0
}

local function resetGame()
    gameState.coins = 0
    gameState.timeLeft = CONFIG.timeLimit
    gameState.isWon = false
    gameState.spawnImmunity = 1.0
    
    reloadMap()
    
    setCameraPosition(CONFIG.spawnPos[1], CONFIG.spawnPos[2], CONFIG.spawnPos[3])
    setCameraRotation(0, 0, -90)
end

resetGame()

addEventHandler("onRender", function(dt)
    local sw, sh = getScreenSize()

    drawText("Coins: " .. gameState.coins .. "/" .. CONFIG.totalCoins, 30, 30, "/assets/fonts/montserrat.ttf", 26, 1, 1, 1, 1, "left")

    if not gameState.isWon then
        gameState.timeLeft = gameState.timeLeft - dt
        drawText(string.format("TIME: %.1f", gameState.timeLeft), sw - 30, 30, "/assets/fonts/montserrat.ttf", 26, 1, 1, 1, 1, "right")

        if gameState.spawnImmunity > 0 then
            gameState.spawnImmunity = gameState.spawnImmunity - dt
            return 
        end

        local px, py, pz = getCameraPosition()
        
        if (pz - 1.8) < CONFIG.lavaLevel or gameState.timeLeft <= 0 then
            resetGame()
            return
        end
    else
        drawText("VICTORY!", sw/2, sh/2 - 50, "/assets/fonts/montserrat.ttf", 64, 0, 1, 0, 1, "center")
    end
end)

addEventHandler("onPickupHit", function(pickupId)
    gameState.coins = gameState.coins + 1
    destroyPickup(pickupId)
    
    if gameState.coins >= CONFIG.totalCoins then 
        gameState.isWon = true
    end
end)
