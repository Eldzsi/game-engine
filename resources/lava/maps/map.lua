object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/lava.png",
    position = {0, 0, -1}, scale = {300, 500, 1},
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
    target = {-3, 8, 1.2}, duration = 3000, pingpong = true
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
    target = {3, 16, 1.4}, duration = 2500, pingpong = true
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

object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {3, 24, 2.0}, scale = {2.0, 2.0, 0.5}
}

object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {-1, 28, 3.0}, scale = {2.0, 2.0, 0.5}
}

pickup {
    model = "/assets/models/coin.obj", texture = "/assets/textures/coin.jpg",
    position = {-1, 28, 4.2},
    type = "coin"
}

movingObject {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png", 
    position = {-4, 32, 3.8}, scale = {2.0, 2.0, 0.5},
    target = {4, 32, 3.8}, duration = 1500, pingpong = true
}

object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {0, 39, 4.0}, scale = {3.0, 3.0, 0.5}
}

pickup {
    model = "/assets/models/coin.obj", texture = "/assets/textures/coin.jpg",
    position = {0, 40, 5.2},
    type = "coin"
}

movingObject {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png", 
    position = {0, 45, 4.0}, scale = {2.5, 2.5, 0.5},
    target = {0, 45, 9.0}, duration = 3000, pingpong = true
}

object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {0, 51, 9.0}, scale = {2.5, 2.5, 0.5},
    glow = {1.0, 0.8, 0.0}
}

movingObject {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png", 
    position = {-6, 57, 9.5}, scale = {2.0, 2.0, 0.5},
    target = {2, 57, 9.5}, duration = 1800, pingpong = true
}

movingObject {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png", 
    position = {6, 62, 10.0}, scale = {2.0, 2.0, 0.5},
    target = {-2, 62, 10.0}, duration = 1800, pingpong = true
}

pickup {
    model = "/assets/models/coin.obj", texture = "/assets/textures/coin.jpg",
    position = {0, 60, 11.5},
    type = "coin"
}

object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {0, 68, 10.0}, scale = {1.5, 1.5, 0.5}
}

object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {3, 72, 11.0}, scale = {1.5, 1.5, 0.5}
}

pickup {
    model = "/assets/models/coin.obj", texture = "/assets/textures/coin.jpg",
    position = {3, 72, 12.2},
    type = "coin"
}

object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {-1, 76, 11.8}, scale = {1.5, 1.5, 0.5}
}

movingObject {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png", 
    position = {0, 82, 11.0}, scale = {2.0, 2.0, 0.5},
    target = {0, 82, 15.0}, duration = 2000, pingpong = true
}

object {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {0, 90, 15.0}, scale = {8.0, 8.0, 1.0},
    glow = {0.2, 0.8, 0.2} 
}

pickup {
    model = "/assets/models/coin.obj", texture = "/assets/textures/coin.jpg",
    position = {0, 90, 16.5},
    type = "finish"
}
