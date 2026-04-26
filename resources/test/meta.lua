name "Test"
author "Lengyel Gábor"
description "Test level"
version "1.0"
type "gamemode"

scripts {
    "scripts/main.lua",
    "scripts/environment.lua",
    "scripts/flashlight.lua",
}

maps {
    "maps/map.lua"
}

dependencies {
    "flashlight"
}

thumbnail "assets/images/thumbnail.jpg"