TARGET=shell
OBJ=src/exec_builtin.o \
		src/exec.o \
    src/main.o \
		src/parser.o \
		src/shell.o \
		src/tokenizer.o

CFLAGS=-Wall -Iinclude

.PHONY: build
build: ${OBJ}
	gcc ${CFLAGS} -o ${TARGET} ${OBJ}

.PHONY: clean
clean:
	rm -rf ${OBJ}
	rm -rf ${TARGET}
