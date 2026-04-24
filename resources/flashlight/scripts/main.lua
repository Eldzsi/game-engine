local flashlight = nil
local isFlashlightOn = false

local config = {
    r = 1.0, g = 1.0, b = 0.8,
    cutoff = 12.5, outerCutoff = 15.0
}

function toggleFlashlight(forceState)
    if type(forceState) == "boolean" then
        isFlashlightOn = forceState
    else
        isFlashlightOn = not isFlashlightOn
    end
    
    if isFlashlightOn then
        if not flashlight then
            local px, py, pz = getCameraPosition()
            local dx, dy, dz = getCameraForward()
            flashlight = createSpotLight(px, py, pz, dx, dy, dz, config.r, config.g, config.b, config.cutoff, config.outerCutoff, 1.0, 0.09, 0.032)
        end
    else
        if flashlight then
            destroySpotLight(flashlight)
            flashlight = nil
        end
    end
    
    return isFlashlightOn
end

function setFlashlightColor(r, g, b)
    if type(r) == "number" then config.r = r else config.r = 1.0 end
    if type(g) == "number" then config.g = g else config.g = 1.0 end
    if type(b) == "number" then config.b = b else config.b = 1.0 end
    return true
end

function setFlashlightSize(innerCutoff, outerCutoff)
    if type(innerCutoff) == "number" then config.cutoff = innerCutoff else config.cutoff = 12.5 end
    if type(outerCutoff) == "number" then config.outerCutoff = outerCutoff else config.outerCutoff = 15.0 end
    
    if isFlashlightOn then
        toggleFlashlight(false)
        toggleFlashlight(true)
    end
    return true
end

addEventHandler("onRender", function(dt)
    if isFlashlightOn and flashlight then
        local px, py, pz = getCameraPosition()
        local dx, dy, dz = getCameraForward()
        
        setSpotLight(flashlight, px, py, pz, dx, dy, dz, config.r, config.g, config.b)
    end
end)
