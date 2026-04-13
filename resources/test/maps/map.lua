object {
    model = "/assets/models/cube.obj",
    texture = "/assets/textures/stone.png",
    position = {0, 0, 0},
    scale = {20.0, 20.0, 0.1}
}

object { 
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {2.5, 0, 0.5} 
}

object { 
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png", 
    position = {-2.5, 0, 0} 
}

object { 
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png", 
    position = {5.0, 0, 1.0}, rotation = {45, 45, 0}
}

movingObject {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {15, 0, 0.25},
    target = {0, 15.0, 0.25}, duration = 2000, loop = false
}

movingObject {
    model = "/assets/models/cube.obj", texture = "/assets/textures/stone.png",
    position = {-8, 10, 0.5},
    target = {-8, -5.0, 0.5}, duration = 3500, loop = true
}