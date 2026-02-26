# Thesis Game Engine (C + Lua)

Name: Lengyel Gábor

Neptun: GKIU70

Summary: A lightweight, data-driven 3D game engine built from scratch using **C, OpenGL and Lua**.
This project was developed as a BSc thesis project to demonstrate low-level graphics programming and scripting integration.

## Technologies & Dependencies

The engine relies on the following standard libraries:
* **OpenGL (Legacy):** 3D rendering API
* **SDL2:** Window creation and input handling
* **SDL2_image:** Texture handling
* **SDL2_mixer:** Audio handling
* **Lua 5.5.0:** Embedded scripting language for logic, map generation and user interface

**Assets (Download and copy the assets/ folder to the root directory):** [Last modification: 2026-02-16]

https://drive.google.com/file/d/1Hy-GlGPFk-HIt-N0c91N4J2MraC7aia9/view?usp=sharing

**Dependencies (Download and copy the `lib/`, `bin/`, and `vendor/` folders to the root directory):** [Last modification: 2026-02-16]

https://drive.google.com/file/d/1Ei84tEMhA8WJvQhv2nmH_nvSTqyWSOyP/view?usp=sharing

**Ready to play version:** [Last modification: 2026-02-16]

https://drive.google.com/file/d/1F-KKk79e7FSkVZZ5vDfbVjtj0b_yY7wd/view?usp=sharing

## Repository Structure

* src/ - The native C core engine (Memory cache, Renderer, Camera, Scripting Bridge).
* include/ - C header files written for the engine.
* assets/scripts/ - Lua scripts defining the game logic.
* vendor/ - External dependencies (e.g. Lua source code, GLAD).
* bin/ - The output directory for the compiled executable.

The program can be compiled using the make command in the terminal.
