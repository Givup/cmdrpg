CC=gcc
LIBS=-luser32 -lkernel32 -lwinmm
DEPS=
SOURCE=./src/map.c ./src/screen.c ./src/perlin.c ./src/status.c ./src/core.c ./src/audio.c ./src/item.c ./src/equipment.c ./src/entity.c ./src/ui.c
MAIN=./src/main.c
INCLUDES=-Iinclude

build:
	$(CC) $(DEPS) -o program.exe $(INCLUDES) $(SOURCE) $(MAIN) $(LIBS)

debug_build:
	$(CC) $(DEPS) -ggdb -o program_debug.exe $(INCLUDES) $(SOURCE) $(MAIN) $(LIBS)

run: build
	./program.exe

