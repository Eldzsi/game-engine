local CONFIG = {
    totalCoins = 5,
    timeLimit = 90.0,
    spawnPos = {0, 0, 4},
    lavaLevel = -0.4,
    platformSpeed = 2.0
}

local gameState = {
    coins = 0,
    timeLeft = CONFIG.timeLimit,
    isWon = false
}

local platforms = {}
local activeCoins = {}

local function clearLevel()
    for _, id in ipairs(platforms) do destroyElement(id) end
    for _, id in ipairs(activeCoins) do destroyPickup(id) end
    platforms = {}
    activeCoins = {}
end

local function spawnLevelContent()
    local platformConfig = {
        {0, 4, 0.8, "static"},   
        {3, 8, 1.2, "moving"},   
        {0, 12, 1.0, "static"}, 
        {-3, 16, 1.4, "moving"}, 
        {0, 20, 1.2, "static"},
    }

    for i, p in ipairs(platformConfig) do
        local platform = createObject("assets/models/cube.obj", "assets/textures/stone.png", p[1], p[2], p[3], 0, 0, 0, 2.5, 2.5, 0.5)
        setElementProperty(platform, "type", p[4])
        setElementProperty(platform, "originX", p[1])
        table.insert(platforms, platform)

        if i <= CONFIG.totalCoins then
            local coin = createPickup("assets/models/coin.obj", "assets/textures/coin.jpg", p[1], p[2], p[3] + 1.2, "stone")
            setObjectGlow(coin, true, 1.0, 0.8, 0.0)
            setObjectMaterial(coin, 1.0, 1.0, 0.5, 120.0)
            table.insert(activeCoins, coin)
        end
    end
end

local function resetGame()
    gameState.coins = 0
    gameState.timeLeft = CONFIG.timeLimit
    gameState.isWon = false
    
    setCameraPosition(CONFIG.spawnPos[1], CONFIG.spawnPos[2], CONFIG.spawnPos[3])
    setCameraRotation(0, 0, -90)
    
    clearLevel()
    spawnLevelContent()
end

local function initLevel()
    local lava = createObject("assets/models/cube.obj", "assets/textures/lava.png", 0, 0, -1, 0, 0, 0, 100, 100, 1)
    setObjectGlow(lava, true, 1.5, 0.6, 0.0)
    setElementCollisionsEnabled(lava, false)

    createObject("assets/models/cube.obj", "assets/textures/stone.png", 0, 0, 0.5, 0, 0, 0, 4, 4, 0.5)
    
    resetGame()
end

addEventHandler("onRender", function(dt)
    local sw, sh = getScreenSize()
    local t = os.clock()

    drawText("Coins: " .. gameState.coins .. "/" .. CONFIG.totalCoins, 30, 30, "assets/fonts/montserrat.ttf", 26, 1, 1, 1, 1, "left")

    if not gameState.isWon then
        gameState.timeLeft = gameState.timeLeft - dt
        drawText(string.format("TIME: %.1f", gameState.timeLeft), sw - 30, 30, "assets/fonts/montserrat.ttf", 26, 1, 1, 1, 1, "right")

        local px, py, pz = getCameraPosition()
        
        if (pz - 1.8) < CONFIG.lavaLevel or gameState.timeLeft <= 0 then
            resetGame()
            return
        end

        for _, id in ipairs(platforms) do
            local mType = getElementProperty(id, "type")
            if mType == "moving" then
                local oX = getElementProperty(id, "originX")
                local _, y, z = getElementPosition(id)
                setElementPosition(id, oX + math.sin(t * CONFIG.platformSpeed) * 3, y, z)
            end
        end
    end
end)

addEventHandler("onPickupHit", function(pickupId)
    gameState.coins = gameState.coins + 1
    destroyPickup(pickupId)
    for i, id in ipairs(activeCoins) do
        if id == pickupId then 
            table.remove(activeCoins, i)
            break 
        end
    end
    
    if gameState.coins >= CONFIG.totalCoins then 
        gameState.isWon = true
    end
end)

initLevel()
