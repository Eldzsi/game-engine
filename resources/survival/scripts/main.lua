function initLevel()
    loadTerrain("assets/heightmaps/heightmap.png", "assets/textures/grass.png", 1.0, 80.0)

    setSkyColor(0.6, 0.78, 0.95)
    setAmbientLight(0.45, 0.40, 0.30)
    setDirectionalLight(0.5, 0.6, -0.8, 1.00, 0.95, 0.80)

    setCameraPosition(140, 150, 30)
    setCameraRotation(0, 0, -90)

    local ambient = playSound("assets/sounds/ambient_bird.wav", true)
    setSoundVolume(ambient, 0.2)
end

initLevel()
