CC=../tcc/tcc.exe
LIBS=-luser32 -lkernel32
SOURCE=./src/map.c ./src/screen.c ./src/perlin.c ./src/main.c
INCLUDES=-Iinclude

build:
	$(CC) -Wall -o program.exe $(INCLUDES) $(SOURCE) $(LIBS)

run: build
	./program.exe

