CC=gcc
LIBS=-luser32 -lkernel32 -lwinmm
DEPS=
SOURCE=./src/map.c ./src/screen.c ./src/perlin.c ./src/status.c
MAIN=./src/main.c
INCLUDES=-Iinclude

build:
	$(CC) $(DEPS) -Wall -o program.exe $(INCLUDES) $(SOURCE) $(MAIN) $(LIBS)

debug_build:
	$(CC) $(DEPS) -Wall -ggdb -o program.exe $(INCLUDES) $(SOURCE) $(MAIN) $(LIBS)

run: build
	./program.exe

