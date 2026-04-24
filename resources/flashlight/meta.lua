name "Flashlight Core"
author "Lengyel Gábor"
description "Provides a player-attached, dynamic spotlight. Exposes API functions to toggle state, adjust color, and modify beam size."
version "1.1"
type "script"

scripts {
    "scripts/main.lua"
}

exports {
    "toggleFlashlight",
    "setFlashlightColor",
    "setFlashlightSize"
}