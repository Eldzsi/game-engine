local sx, sy = getScreenSize()

addEventHandler("onRender", function()
    drawRectangle(sx/2 - 2, sy/2 - 2, 4, 4, 1, 0, 0, 0.8)
end)