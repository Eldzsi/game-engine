object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/lava.png",
    position = {0, 0, -1}, scale = {100, 100, 1},
    glow = {1.5, 0.6, 0.0},
    collisions = false,
    uvSpeed = {0.1, 0.03}
}

object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {0, 0, 0.5}, scale = {4, 4, 0.5}
}

object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png", 
    position = {0, 4, 0.8}, scale = {2.5, 2.5, 0.5}
}

pickup {
    model = "/assets/models/coin.obj", texture = "/assets/textures/coin.jpg",
    position = {0, 4, 2.0},
    type = "coin"
}

movingObject {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png", 
    position = {3, 8, 1.2}, scale = {2.5, 2.5, 0.5},
    target = {-3, 8, 1.2}, duration = 3000, loop = true
}

pickup {
    model = "/assets/models/coin.obj", texture = "/assets/textures/coin.jpg",
    position = {3, 8, 2.4},
    type = "coin"
}

object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {0, 12, 1.0}, scale = {2.5, 2.5, 0.5}
}

pickup {
    model = "/assets/models/coin.obj", texture = "/assets/textures/coin.jpg",
    position = {0, 12, 2.2},
    type = "coin"
}

movingObject {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png", 
    position = {-3, 16, 1.4}, scale = {2.5, 2.5, 0.5},
    target = {3, 16, 1.4}, duration = 2500, loop = true
}

pickup {
    model = "/assets/models/coin.obj", texture = "/assets/textures/coin.jpg",
    position = {-3, 16, 2.6},
    type = "coin"
}

object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {0, 20, 1.2}, scale = {2.5, 2.5, 0.5}
}

pickup {
    model = "/assets/models/coin.obj", texture = "/assets/textures/coin.jpg",
    position = {0, 20, 2.4},
    type = "coin"
}