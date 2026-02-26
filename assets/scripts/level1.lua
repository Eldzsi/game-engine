local level_objects = {
    {model = "assets/models/cube.obj", x = 0,    y = 0, z = 0.5,   rx = 0,  ry = 0,  rz = 0},
    {model = "assets/models/cube.obj", x = 2.5,  y = 0, z = 0.5,   rx = 0,  ry = 0,  rz = 0},
    {model = "assets/models/cube.obj", x = -2.5, y = 0, z = 0.5,   rx = 0,  ry = 0,  rz = 0},
    {model = "assets/models/cube.obj", x = 0,    y = 0, z = 2.5,   rx = 45,   ry = 45,   rz = 0}
}

for _, obj in pairs(level_objects) do
    createObject(obj.model, obj.x, obj.y, obj.z, obj.rx, obj.ry, obj.rz)
end