# Thesis Game Engine (C + Lua)

Name: Lengyel Gábor

Neptun: GKIU70

Summary: A lightweight, data-driven 3D game engine built from scratch using **C, OpenGL and Lua**.
This project was developed as a BSc thesis project to demonstrate low-level graphics programming and scripting integration.

## Technologies & Dependencies

The engine relies on the following standard libraries:
* **OpenGL:** 3D rendering API
* **SDL2:** Window creation and input handling
* **SDL2_image:** Texture handling
* **SDL2_mixer:** Audio handling
* **SDL2_ttf:** Font handling
* **cglm:** Mathematics library for graphics programming
* **fast_obj:** Fast C/C++ single header OBJ model loader
* **Lua 5.5.0:** Embedded scripting language for logic, map generation and user interface

**Assets (Download and copy the assets/ folder to the root directory):** [Last modification: 2026-04-24]

https://drive.google.com/file/d/1nty2YmvIhNInGq1Rb6-UTNyI_GZCh7zH/view?usp=sharing

**Dependencies (Download and copy the lib/, bin/, and vendor/ folders to the root directory):** [Last modification: 2026-04-24]

https://drive.google.com/file/d/1DV_bsh0U-Mi9vfTQ2JHg78GKE0untdHJ/view?usp=sharing

**Resources (Download and copy the resources/ folder to the root directory):** [Last modification: 2026-04-26]

https://drive.google.com/file/d/1y1DoBl088KmEpLaR1tNjw_iZU-orgZyx/view?usp=sharing

**Ready to play version:** [Last modification: 2026-04-26]

https://drive.google.com/file/d/1NUDRyYOS2n-mHgLRZ07UnfpKAC3koLiX/view?usp=sharing

## Project Structure

* src/ - The native C core engine (Memory cache, Renderer, Camera, Scripting Bridge).
* include/ - C header files written for the engine.
* lua_core/ - The core Lua API wrappers bridging C functions to the scripting environment.
* vendor/ - External dependencies (e.g. Lua source code, GLAD).
* bin/ - The output directory for the compiled executable.
* resources/ - Independent game modules (levels, game modes) containing their own meta.lua and logic scripts.

## Compilation

The program can be compiled using the **make** command in the terminal.