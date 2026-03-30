local flashlight
local isFlashlightOn = false

addEventHandler("onKey", function(key, state)
    if key == "F" and state == true then
        isFlashlightOn = not isFlashlightOn
        
        if not flashlight  then
            flashlight = createSpotLight(0, 0, 0, 0, 0, -1, 1, 1, 1, 12.5, 17.5, 1.0, 0.09, 0.032)
        end
    end
end)

addEventHandler("onRender", function(dt)
    if flashlight then
        if isFlashlightOn then
            local px, py, pz = getCameraPosition()
            local dx, dy, dz = getCameraForward()
            setSpotLight(flashlight, px, py, pz, dx, dy, dz, 1.0, 1.0, 0.8)
        else
            setSpotLight(flashlight, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        end
    end
end)