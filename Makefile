all:
	@mkdir -p bin
	gcc -Iinclude -Ivendor/include -Ivendor -Ivendor/lua -Llib src/app.c src/camera.c src/main.c src/model.c src/scene.c src/scripting.c src/sound.c src/texture.c src/utils.c vendor/glad.c vendor/lua/*.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lopengl32 -mconsole -o bin/game.exe -Wall -Wextra

linux:
	@mkdir -p bin
	gcc -Iinclude -Ivendor/include -Ivendor -Ivendor/lua src/app.c src/camera.c src/main.c src/model.c src/scene.c src/scripting.c src/sound.c src/texture.c src/utils.c vendor/glad.c vendor/lua/*.c -lSDL2 -lSDL2_image -lSDL2_mixer -lGL -lm -o bin/game -Wall -Wextra