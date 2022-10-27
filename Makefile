TARGET=shell

CFLAGS=-Wall
SRC=main.c
OBJ=main.o

.PHONY: build
build: ${OBJ}
	gcc ${CFLAGS} -o shell ${OBJ}
