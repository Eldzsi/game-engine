local activeFires = {}
local fireCounter = 0

function createFire(x, y, z, density, scale)
    density = density or 15
    scale = scale or 1.0      
    
    fireCounter = fireCounter + 1
    activeFires[fireCounter] = {x = x, y = y, z = z, density = density, scale = scale}
    
    return fireCounter
end

function destroyFire(id)
    if activeFires[id] then
        activeFires[id] = nil
    end
end

addEventHandler("onRender", function()
    for id, fire in pairs(activeFires) do
        local count = math.floor(fire.density)
        local fraction = fire.density - count
        
        if math.random() < fraction then
            count = count + 1
        end

        for i = 1, count do
            local spawnX = fire.x + (math.random() - 0.5) * 1.0 * fire.scale
            local spawnY = fire.y + (math.random() - 0.5) * 1.0 * fire.scale
            local spawnZ = fire.z 

            local vx = (math.random() - 0.5) * 0.4 * fire.scale
            local vy = (math.random() - 0.5) * 0.4 * fire.scale
            local vz = (math.random() * 1.5 + 1.0) * fire.scale 

            local life = math.random() * 0.4 + 0.4
            local size = (math.random() * 0.5 + 0.5) * fire.scale
            
            emitParticle(spawnX, spawnY, spawnZ, vx, vy, vz, 1.0, 0.4, 0.05, 1.0, life, size)
        end
    end
end)
