local platformData = {
    {model = "assets/models/cube.obj", tex = "assets/textures/stone.png", x = 2.5, y = 0, z = 0.5, rx = 0, ry = 0, rz = 0},
    {model = "assets/models/cube.obj", tex = "assets/textures/stone.png", x = -2.5, y = 0, z = 0, rx = 0, ry = 0, rz = 0},
    {model = "assets/models/cube.obj", tex = "assets/textures/stone.png", x = 5.0, y = 0, z = 1.0, rx = 45, ry = 45, rz = 0},

    {model = "assets/models/cube.obj", tex = "assets/textures/stone.png", x = 0, y = 0, z = 0.25, rx = 0, ry = 0, rz = 0, 
    isMoving = true, targetX = 0, targetY = 15.0, targetZ = 0.25, duration = 2000},
     
    {model = "assets/models/cube.obj", tex = "assets/textures/stone.png", x = -8, y = 10, z = 0.5, rx = 0, ry = 0, rz = 0, 
    isMoving = true, targetX = -8, targetY = -5.0, targetZ = 0.5, duration = 3500}
}

local movingPlatforms = {}

local floor = createObject("assets/models/cube.obj", "assets/textures/stone.png", 0, 0, 0, 0, 0, 0, 20.0, 20.0, 0.1)
setObjectMaterial(floor, 0.3, 0.3, 0.3, 100.0)

function createMovingPlatform(data)
    local platform = createObject(data.model, data.tex, data.x, data.y, data.z, data.rx, data.ry, data.rz)
    
    setElementProperty(platform, "startX", data.x)
    setElementProperty(platform, "startY", data.y)
    setElementProperty(platform, "startZ", data.z)
    
    setElementProperty(platform, "targetX", data.targetX)
    setElementProperty(platform, "targetY", data.targetY)
    setElementProperty(platform, "targetZ", data.targetZ)
    
    setElementProperty(platform, "duration", data.duration)
    setElementProperty(platform, "movingToTarget", true)

    table.insert(movingPlatforms, platform)

    return platform
end

for _, data in ipairs(platformData) do
    if data.isMoving then
        createMovingPlatform(data)
    else
        createObject(data.model, data.tex, data.x, data.y, data.z, data.rx, data.ry, data.rz)
    end
end

setTimer(function()
    for _, id in ipairs(movingPlatforms) do
        local movingToTarget = getElementProperty(id, "movingToTarget")
        local dur = getElementProperty(id, "duration")

        if movingToTarget then
            local tx = getElementProperty(id, "targetX")
            local ty = getElementProperty(id, "targetY")
            local tz = getElementProperty(id, "targetZ")
            moveObject(id, dur, tx, ty, tz, 0, 0, 0)
            setElementProperty(id, "movingToTarget", false)
        else
            local sx = getElementProperty(id, "startX")
            local sy = getElementProperty(id, "startY")
            local sz = getElementProperty(id, "startZ")
            moveObject(id, dur, sx, sy, sz, 0, 0, 0)
            setElementProperty(id, "movingToTarget", true)
        end
    end
end, 2000, 0)

function drawCrosshair()
    local sx, sy = getScreenSize()
    drawRectangle((sx/2)-2, (sy/2)-2, 4, 4, 1.0, 0.0, 0.0, 1.0)
end
addEventHandler("onRender", drawCrosshair)
