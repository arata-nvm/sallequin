TARGET=shell

CFLAGS=-Wall -Iinclude
OBJ=src/exec_builtin.o \
		src/exec.o \
    src/main.o \
		src/parser.o \
		src/shell.o

.PHONY: build
build: ${OBJ}
	gcc ${CFLAGS} -o ${TARGET} ${OBJ}

.PHONY: clean
clean:
	rm -rf ${OBJ}
	rm -rf ${TARGET}
