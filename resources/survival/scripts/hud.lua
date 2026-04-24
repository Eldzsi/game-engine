local sx, sy = getScreenSize()

local function drawStatBar(x, y, width, height, percent, color, label)
    drawRectangle(x, y, width, height, 0, 0, 0, 0.6)
    
    local fillWidth = (width - 4) * (percent / 100)
    if fillWidth < 0 then fillWidth = 0 end
    
    drawRectangle(x + 2, y + 2, fillWidth, height - 4, color.r, color.g, color.b, 0.8)
    
    drawText(label, x + 5, y - 18, "/assets/fonts/montserrat.ttf", 14, 1, 1, 1, 1, "left")
end

addEventHandler("onPostRender", function()
    sx, sy = getScreenSize()
    
    local hp = getElementData(localPlayer, "health") or 0
    local hunger = getElementData(localPlayer, "hunger") or 0
    local stamina = getElementData(localPlayer, "stamina") or 0
    
    local startX = 20
    local startY = sy - 110
    local barW = 200
    local barH = 15
    local spacing = 35
    
    drawStatBar(startX, startY, barW, barH, hp, {r=0.8, g=0.2, b=0.2}, "HEALTH")
    
    drawStatBar(startX, startY + spacing, barW, barH, hunger, {r=0.8, g=0.5, b=0.2}, "HUNGER")
    
    drawStatBar(startX, startY + (spacing * 2), barW, barH, stamina, {r=0.2, g=0.7, b=0.3}, "STAMINA")
end)
