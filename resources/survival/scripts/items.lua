ITEMS = {
    ["stick"]           = {name = "Stick", type = "resource", maxStack = 64, icon = "assets/icons/stick.png", description = "A simple piece of wood, useful for tool handles."},
    ["wood"]            = {name = "Wood", type = "resource", maxStack = 64, icon = "assets/icons/wood.png", description = "Solid wooden log, excellent for fuel or building."},
    ["stone"]           = {name = "Stone", type = "resource", maxStack = 64, icon = "assets/icons/stone.png", description = "Solid piece of stone, essential for tools and campfires."},
    ["berries"]         = {name = "Berries", type = "food", nutrition = 15, maxStack = 32, icon = "assets/icons/berries.png", description = "Fresh forest berries. Slightly satisfies hunger."},
    ["mushroom"]        = {name = "Mushroom", type = "food", nutrition = 10, maxStack = 32, icon = "assets/icons/mushroom.png", description = "Smells a bit weird, but edible raw."},
    ["cooked_mushroom"] = {name = "Cooked Mushroom", type = "food", nutrition = 30, maxStack = 32, icon = "assets/icons/cooked_mushroom.png", description = "Mushroom cooked over a fire. Much tastier and more nutritious than raw."},
    
    ["stone_axe"]  = {
        name = "Stone Axe", 
        type = "tool", 
        maxStack = 1,
        icon = "assets/icons/stone_axe.png",
        description = "Primitive stone axe for chopping trees.",
        model = "assets/models/stone_axe.obj",
        texture = {"assets/textures/axe_wood.jpg", "assets/textures/axe_stone.jpg"},
        handRotation = {rx = 0, ry = 180, rz = -90},
        handOffset = {right = 0.4, up = -0.35, forward = 0.55} ,
        handScale = 0.1
   },
    
    ["pickaxe"] = {
        name = "Pickaxe",
        type = "tool",
        maxStack = 1,
        icon = "assets/icons/pickaxe.png",
        description = "Simple pickaxe for mining rocks and stones.",
        model = "assets/models/pickaxe.obj", 
        texture = {"assets/textures/pick_wood.jpg", "assets/textures/pick_stone.jpg"},
        handRotation = {rx = 0, ry = 80, rz = -90},
        handOffset = {right = 0.45, up = -0.1, forward = 0.6},
        handScale = 0.1
   },
    
    ["campfire"] = {
        name = "Campfire",
        type = "placeable",
        maxStack = 5,
        icon = "assets/icons/campfire.png",
        description = "Provides warmth, light, and you can cook your food on it.",
        model = "assets/models/campfire.obj", 
        texture = {
            "assets/textures/campfire_stone.png",
            "assets/textures/campfire_stone_02.png",
            "assets/textures/campfire_wood.png",
        },
        placeScale = 0.1, 
        placeRotation = {rx = 90, ry = 0, rz = 0}
   }
}

RECIPES = {
    ["stone_axe"] = {
        requires = {
            {item = "stick", count = 2}, 
            {item = "stone", count = 1}
       }, 
        yields = 1 
   },
    ["pickaxe"] = {
        requires = {
            {item = "stick", count = 2}, 
            {item = "stone", count = 2}
       }, 
        yields = 1 
   },
    ["campfire"] = {requires = {{item = "wood", count = 3}, {item = "stone", count = 3}}, yields = 1},

    ["cooked_mushroom"] = {
        requires = {{item = "mushroom", count = 1}},
        nearObject = "campfire", 
        nearDistance = 4.0
   },
}
