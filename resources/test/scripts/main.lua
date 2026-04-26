function drawCrosshair()
    local sx, sy = getScreenSize()
    drawRectangle((sx/2)-2, (sy/2)-2, 4, 4, 1.0, 0.0, 0.0, 1.0)
end
--addEventHandler("onRender", drawCrosshair)