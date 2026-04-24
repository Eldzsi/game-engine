local playerStats = {
    health = 100.0,
    hunger = 100.0,
    stamina = 100.0,
}

local RATES = {
    hungerLoss = 0.3,
    tempLoss = 0.15,
    
    staminaRegen = 10.0,
    staminaDrainSprint = 5.0,
    staminaDrainJump = 15.0,
    
    starveDamage = 1.5,
}

local isDead = false
local isSprintingKeyHeld = false 

function modifyStat(statName, amount)
    if playerStats[statName] and not isDead then
        playerStats[statName] = math.max(0, math.min(100, playerStats[statName] + amount))
        setElementData(localPlayer, statName, playerStats[statName])
        return true
    end
    
    return false
end

addEventHandler("onKey", function(key, state)
    if isDead then
        return
    end

    if key == "Left Shift" or key == "LShift" or key == "lshift" then
        isSprintingKeyHeld = state
    end

    if key == "Space" and state == true then
        if playerStats.stamina < RATES.staminaDrainJump then
        else
            playerStats.stamina = playerStats.stamina - RATES.staminaDrainJump
        end
    end
end)

addEventHandler("onRender", function(dt)
    if isDead then
        return
    end
    dt = dt or 0.016

    playerStats.hunger = math.max(0, playerStats.hunger - (RATES.hungerLoss * dt))
        
    if playerStats.stamina <= 0 then
        toggleControl("sprint", false)
        setControlState("sprint", false) 
    elseif playerStats.stamina > 10 then 
        toggleControl("sprint", true)
    end

    if playerStats.stamina < RATES.staminaDrainJump then
        toggleControl("jump", false)
    else
        toggleControl("jump", true)
    end

    if isSprintingKeyHeld and playerStats.stamina > 0 then
        playerStats.stamina = math.max(0, playerStats.stamina - (RATES.staminaDrainSprint * dt))
    else
        playerStats.stamina = math.min(100, playerStats.stamina + (RATES.staminaRegen * dt))
    end
    
    if playerStats.hunger <= 0 then
        playerStats.health = playerStats.health - (RATES.starveDamage * dt)
    end
    
    if playerStats.health <= 0 then
        playerStats.health = 0
        isDead = true
        if triggerEvent then triggerEvent("onPlayerDeath", "root") end
    end
    
    setElementData(localPlayer, "health", playerStats.health)
    setElementData(localPlayer, "hunger", playerStats.hunger)
    setElementData(localPlayer, "stamina", playerStats.stamina)
end)
