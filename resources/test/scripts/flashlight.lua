addEventHandler("onKey", function(key, state)
    if key == "F" and state == true then
        exports.flashlight.toggleFlashlight()
    end
end)