all:
	@mkdir -p bin
	gcc -Iinclude -Ivendor/include -Ivendor -Ivendor/lua -Llib \
	src/app.c src/camera.c src/main.c src/model.c src/scene.c \
	src/collision.c src/scripting.c src/shader.c src/sound.c src/texture.c \
	src/ui.c src/particle.c src/utils.c vendor/glad.c vendor/lua/*.c \
	-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lopengl32 \
	-mconsole -o bin/game.exe -Wall -Wextra

linux:
	@mkdir -p bin
	gcc -Iinclude -Ivendor/include -Ivendor -Ivendor/lua \
	src/app.c src/camera.c src/main.c src/model.c src/scene.c \
	src/collision.c src/scripting.c src/shader.c src/sound.c src/texture.c \
	src/ui.c src/particle.c src/utils.c vendor/glad.c vendor/lua/*.c \
	-lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lGL -lm \
	-o bin/game -Wall -Wextra