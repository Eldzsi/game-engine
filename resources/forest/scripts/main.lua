local activeFires = {}

function createFireWithLight(x, y, z, density, size)
    local fire = createFire(x, y, z, density, size) 
    local light = createPointLight(x, y, z + 0.8, 1.0, 0.5, 0.0, 1.0, 0.09, 0.032)
    
    table.insert(activeFires, {fireId = fire, lightId = light})
    return fireId
end

local floor = createObject("assets/models/cube.obj", "assets/textures/dead_ground.png", 0, 0, 0, 0, 0, 0, 100.0, 100.0, 0.1)

local trunk_assets = {
    {obj = "assets/models/trunk_01.obj", tex = "assets/textures/trunk_01.png"},
    {obj = "assets/models/trunk_02.obj", tex = "assets/textures/trunk_02.png"}
}

math.randomseed(os.time()) 

for i = 1, 20 do
    local type_idx = math.random(1, 2)
    local model = trunk_assets[type_idx].obj
    local texture = trunk_assets[type_idx].tex

    local px = math.random(-90, 90) / 10.0
    local py = math.random(-90, 90) / 10.0
    local pz = 0.5 

    local rz = math.random(0, 360)
    local rx = math.random(85, 95)
    
    local scale = math.random(8, 15) / 10.0

    createObject(model, texture, px, py, pz, rx, 0, rz, scale, scale, scale)
end

createFireWithLight(-3, 2, 0, 1, 1)
createFireWithLight(4, -5, 0, 1, 1)
createFireWithLight(0, 0, 0, 1, 1)
createFireWithLight(-7, -7, 0, 1, 1)

addEventHandler("onRender", function()
    local time = os.clock()
    
    for _, fire in ipairs(activeFires) do
        local flicker = (math.sin(time * 10.0) * 0.1) + (math.random(90, 110) / 100.0)
        
        setPointLight(fire.lightId, nil, nil, nil, 1.0 * flicker, 0.4 * flicker, 0.0)
    end
end)
