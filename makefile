CC=gcc
LIBS=-luser32 -lkernel32 -lwinmm
DEPS=
SOURCE=./src/map.c ./src/screen.c ./src/perlin.c ./src/main.c
INCLUDES=-Iinclude

build:
	$(CC) $(DEPS) -Wall -o program.exe $(INCLUDES) $(SOURCE) $(LIBS)

run: build
	./program.exe

