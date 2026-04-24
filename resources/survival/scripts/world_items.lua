local spawnedTrees = {}

function spawnTree(x, y, z)
    local model = "assets/models/tree.obj"
    local treeTextures = {
        "assets/textures/bark.jpg",
        "assets/textures/leaves.png"
    }
    
    z = z or getTerrainHeight(x, y)
    
    local scale = math.random(15, 28) / 10.0
    
    local id = createObject(model, treeTextures, x, y, z, 90, math.random(0, 360), 0, scale, scale, scale)
    
    if id then
        setElementColliderScale(id, 0.2 * scale, 1.0 * scale, 0.2 * scale)
        
        setElementData(id, "item:id", "tree")
        setElementData(id, "item:name", "Tree")
        setElementData(id, "tree:hp", 100) 
        
        table.insert(spawnedTrees, id)
        return id
    end
    
    return false
end

function initWorldItems()
    math.randomseed(os.time())
    
    local treeCount = 0
    local targetTrees = 100
    
    local mapSize = 1024 
    local halfMap = mapSize / 2
    
    local minHeight = 5.0   
    local maxHeight = 80.0  
        
    local attempts = 0
    while treeCount < targetTrees and attempts < 2000 do
        attempts = attempts + 1
        
        local randX = math.random(-halfMap, halfMap)
        local randY = math.random(-halfMap, halfMap)
        local z = getTerrainHeight(randX, randY)
        
        if z >= minHeight and z <= maxHeight then
            if spawnTree(randX, randY, z) then
                treeCount = treeCount + 1
            end
        end
    end

    for i = 1, 200 do
        local rx = math.random(-halfMap, halfMap)
        local ry = math.random(-halfMap, halfMap)
        spawnGroundResource("stick", rx, ry)
    end

    for i = 1, 100 do
        local rx = math.random(-halfMap, halfMap)
        local ry = math.random(-halfMap, halfMap)
        spawnGroundResource("stone", rx, ry)
    end

    for i = 1, 60 do
        local rx = math.random(-halfMap, halfMap)
        local ry = math.random(-halfMap, halfMap)
        spawnGroundResource("mushroom", rx, ry)
    end

    for i = 1, 50 do
        local rx = math.random(-halfMap, halfMap)
        local ry = math.random(-halfMap, halfMap)
        spawnRockNode(rx, ry)
    end
end

local fallingTrees = {}

function fallTree(treeId)
    setElementCollisionsEnabled(treeId, false)
    
    local rx, ry, rz = getElementRotation(treeId)
    
    table.insert(fallingTrees, {
        id = treeId,
        startRx = rx,
        ry = ry,
        rz = rz,
        progress = 0.0,
        speed = 0.18
    })
end

addEventHandler("onRender", function(dt)
    dt = dt or 0.016 
    
    for i = #fallingTrees, 1, -1 do
        local anim = fallingTrees[i]
        
        anim.progress = anim.progress + (dt * anim.speed)
        
        if anim.progress >= 1.0 then
            local tx, ty, tz = getElementPosition(anim.id)
            
            destroyElement(anim.id)
            table.remove(fallingTrees, i)
            
            for j = 1, 2 do
                local ox = (math.random() - 0.5) * 1.5
                local oy = (math.random() - 0.5) * 1.5
                local rz = math.random(0, 360)
                
                local logId = createObject(
                    "/assets/models/trunk_01.obj", 
                    "/assets/textures/trunk_01.png", 
                    tx + ox, ty + oy, tz, 
                    0, 0, rz,
                    0.8, 0.8, 0.8
                )
                
                if logId then
                    setElementData(logId, "item:id", "wood")
                    setElementData(logId, "item:name", "Wood Log")
                end
            end
        else
            local currentFall = 90 * (anim.progress * anim.progress)
            setElementRotation(anim.id, anim.startRx - currentFall, anim.ry, anim.rz)
        end
    end
end)

function spawnGroundResource(type, x, y)
    local model, texture, scale
    
    if type == "stick" then
        model = "/assets/models/trunk_01.obj"
        texture = "/assets/textures/trunk_01.png"
        scale = 0.5
    elseif type == "stone" then
        model = "assets/models/rock.obj"
        texture = "assets/textures/rock.jpg"
        scale = 0.2
    elseif type == "mushroom" then
        model = "assets/models/mushroom.obj"
        texture = "assets/textures/mushroom.jpg"
        scale = 0.1
    end
    
    local z = getTerrainHeight(x, y)
    local id = createObject(model, texture, x, y, z, 0, 0, math.random(0, 360), scale, scale, scale)
    
    if id then
        setElementData(id, "item:id", type)
        setElementData(id, "item:name", ITEMS[type].name)

        return id
    end
end

function spawnRockNode(x, y)
    local z = getTerrainHeight(x, y)
    local scale = math.random(12, 25) / 10.0
    
    local id = createObject("assets/models/rock.obj", "assets/textures/rock.jpg", x, y, z, 0, math.random(0, 360), 0, scale, scale, scale)
    
    if id then
        setElementColliderScale(id, 0.7 * scale, 0.7 * scale, 0.7 * scale)
        setElementData(id, "item:id", "rock_node")
        setElementData(id, "item:name", "Rock Node")
        setElementData(id, "rock:hp", 100)
    end
end

function breakRock(rockId)
    local x, y, z = getElementPosition(rockId)
    
    destroyElement(rockId) 
    
    local count = math.random(2, 4)
    for i = 1, count do
        local ox = (math.random() - 0.5) * 2.0
        local oy = (math.random() - 0.5) * 2.0
        local rz = math.random(0, 360)
        
        local stoneId = createObject(
            "assets/models/rock.obj", 
            "assets/textures/rock.jpg", 
            x + ox, y + oy, z, 
            0, 0, rz, 
            0.2, 0.2, 0.2
        )
        
        if stoneId then
            setElementData(stoneId, "item:id", "stone")
            setElementData(stoneId, "item:name", "Stone")
        end
    end
end

initWorldItems()
