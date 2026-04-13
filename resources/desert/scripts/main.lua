function initLevel()
    loadTerrain("/assets/textures/heightmap.png", "/assets/textures/sand.png", 2.0, 30.0)
    local x, y, z = 128.0, 128.0, 50
    setCameraPosition(x, y, z)
    setCameraRotation(0, 0, -90)
end

initLevel()